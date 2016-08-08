#ifndef GALOIS_RUNTIME_KDG_SPEC_LOCAL_MIN_H
#define GALOIS_RUNTIME_KDG_SPEC_LOCAL_MIN_H

#include "Galois/Runtime/IKDGbase.h"
#include "Galois/Runtime/WindowWorkList.h"
#include "Galois/Runtime/Executor_ParaMeter.h"

namespace Galois {
namespace Runtime {

template <typename T, typename Cmp, typename NhFunc, typename OpFunc, typename ArgsTuple>
class KDGspecLocalMinExecutor: public IKDGbase<T, Cmp, NhFunc, HIDDEN::DummyExecFunc, OpFunc, ArgsTuple, TwoPhaseContext<T, Cmp> >{

protected:

  using Ctxt = TwoPhaseContext<T, Cmp>;
  using Base = IKDGbase <T, Cmp, NhFunc, HIDDEN::DummyExecFunc, OpFunc, ArgsTuple, Ctxt>;

  using WindowWL = typename std::conditional<Base::NEEDS_PUSH, PQwindowWL<T, Cmp>, SortedRangeWindowWL<T, Cmp> >::type;
  using CtxtWL = typename Base::CtxtWL;



  WindowWL winWL;
  PerThreadBag<T> pending;
  CtxtWL scheduled;

public:

  KDGspecLocalMinExecutor (
      const Cmp& cmp, 
      const NhFunc& nhFunc,
      const OpFunc& opFunc,
      const ArgsTuple& argsTuple)
    :
      Base (cmp, nhFunc, HIDDEN::DummyExecFunc (), opFunc, argsTuple),
      winWL (cmp)
  {}

  ~KDGspecLocalMinExecutor () {

    dumpStats ();

    if (Base::ENABLE_PARAMETER) {
      ParaMeter::closeStatsFile ();
    }
  }


protected:
  void dumpStats (void) {
    reportStat (Base::loopname, "efficiency %", double (100.0 * Base::totalCommits) / Base::totalTasks,0);
    reportStat (Base::loopname, "avg. parallelism", double (Base::totalCommits) / Base::rounds,0);
  }

  void abortCtxt (Ctxt* c) {
    assert (c);
    c->cancelIteration ();
    c->reset ();
    c->~Ctxt ();
    Base::ctxtAlloc.deallocate (c, 1);
  }


  void expandNhood (void) {

    Galois::do_all_choice (makeLocalRange (pending),
        [this] (const T& x) {

          Ctxt* c = Base::ctxtAlloc.allocate (1);
          assert (c);
          Base::ctxtAlloc.construct (c, x, Base::cmp);

          typename Base::UserCtxt& uhand = *Base::userHandles.getLocal ();
          uhand.reset ();
          runCatching (Base::nhFunc, c, uhand);

          if (c->isSrc ()) {
            scheduled.push (c);
          } else {
            abortCtxt (c);
          }

          Base::roundTasks += 1;
        },
        std::make_tuple (
          Galois::loopname ("expandNhood"),
          chunk_size<NhFunc::CHUNK_SIZE> ()));

    pending.clear_all_parallel ();
  }

  void applyOperator (void) {

    Galois::optional<T> minElem;

    if (Base::NEEDS_PUSH) {
      if (Base::targetCommitRatio != 0.0 && !winWL.empty ()) {
        minElem = *winWL.getMin();
      }
    }


    Galois::do_all_choice (makeLocalRange (scheduled),
        [this, &minElem] (Ctxt* c) {

          typename Base::UserCtxt& uhand = *Base::userHandles.getLocal ();
          uhand.reset ();

          if (c->isSrc ()) {
            static_assert (!Base::OPERATOR_CAN_ABORT, "operator not allowed to abort");
            Base::opFunc (c->getActive (), uhand);
            Base::roundCommits += 1;

            if (Base::NEEDS_PUSH) { 
              for (auto i = uhand.getPushBuffer ().begin ()
                  , endi = uhand.getPushBuffer ().end (); i != endi; ++i) {

                if ((Base::targetCommitRatio == 0.0) || !minElem || !Base::cmp (*minElem, *i)) {
                  // if *i >= *minElem
                  pending.push (*i);;
                } else {
                  winWL.push (*i);
                } 
              }
            } else {
              assert (uhand.getPushBuffer ().begin () == uhand.getPushBuffer ().end ());
            }

            c->commitIteration ();
            c->~Ctxt ();
            Base::ctxtAlloc.deallocate (c, 1);

          } else {
            abortCtxt (c);
          }
        },
        std::make_tuple (
          Galois::loopname ("applyOperator"),
          chunk_size<OpFunc::CHUNK_SIZE> ()));

    scheduled.clear_all_parallel ();
  }

  GALOIS_ATTRIBUTE_PROF_NOINLINE void endRound () {

    if (Base::ENABLE_PARAMETER) {
      ParaMeter::StepStats s (Base::rounds, Base::roundCommits.reduceRO (), Base::roundTasks.reduceRO ());
      s.dump (ParaMeter::getStatsFile (), Base::loopname);
    }

    Base::endRound ();
  }

public:

  template <typename R>
  void push_initial (const R& range) {

    if (Base::targetCommitRatio == 0.0) {

      Galois::do_all_choice (range,
          [this] (const T& x) {
            pending.push (x);
          }, 
          std::make_tuple (
            Galois::loopname ("init-fill"),
            chunk_size<NhFunc::CHUNK_SIZE> ()));


    } else {
      winWL.initfill (range);
    }
  }

  void execute () {

    while (true) {

      Base::refillRound (winWL, pending);

      if (pending.empty_all()) {
        break;
      }

      expandNhood ();

      applyOperator ();

      endRound ();

    }

  }
};

template <typename R, typename Cmp, typename NhFunc, typename OpFunc, typename _ArgsTuple>
void for_each_ordered_kdg_spec_local_min_impl (const R& range, const Cmp& cmp, const NhFunc& nhFunc, 
    const OpFunc& opFunc, const _ArgsTuple& argsTuple) {

  auto argsT = std::tuple_cat (argsTuple, 
      get_default_trait_values (argsTuple,
        std::make_tuple (loopname_tag {}, enable_parameter_tag {}),
        std::make_tuple (default_loopname {}, enable_parameter<false> {})));
  using ArgsT = decltype (argsT);

  using T = typename R::value_type;
  

  using Exec = KDGspecLocalMinExecutor<T, Cmp, NhFunc, OpFunc, ArgsT>;
  
  Exec e (cmp, nhFunc, opFunc, argsT);

  const bool wakeupThreadPool = true;

  if (wakeupThreadPool) {
    Substrate::ThreadPool::getThreadPool().burnPower(Galois::getActiveThreads ());
  }

  e.push_initial (range);
  e.execute ();

  if (wakeupThreadPool) {
    Substrate::ThreadPool::getThreadPool().beKind ();
  }

}

template <typename R, typename Cmp, typename NhFunc, typename OpFunc, typename _ArgsTuple>
void for_each_ordered_kdg_spec_local_min (const R& range, const Cmp& cmp, const NhFunc& nhFunc, 
    const OpFunc& opFunc, const _ArgsTuple& argsTuple) {

  auto tplParam = std::tuple_cat (argsTuple, std::make_tuple (enable_parameter<true> ()));
  auto tplNoParam = std::tuple_cat (argsTuple, std::make_tuple (enable_parameter<false> ()));

  if (useParaMeterOpt) {
    for_each_ordered_kdg_spec_local_min_impl (range, cmp, nhFunc, opFunc, tplParam);
  } else {
    for_each_ordered_kdg_spec_local_min_impl (range, cmp, nhFunc, opFunc, tplNoParam);
  }
}

} // close Runtime
} // close Galois



#endif // GALOIS_RUNTIME_KDG_SPEC_LOCAL_MIN_H