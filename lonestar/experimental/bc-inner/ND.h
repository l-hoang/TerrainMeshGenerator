/** Async Betweenness centrality Node  -*- C++ -*-
 * @file
 * @section License
 *
 * Galois, a framework to exploit amorphous data-parallelism in irregular
 * programs.
 *
 * Copyright (C) 2018, The University of Texas at Austin. All rights reserved.
 * UNIVERSITY EXPRESSLY DISCLAIMS ANY ABCNode ALL WARRANTIES CONCERNING THIS
 * SOFTWARE ABCNode DOCUMENTATION, INCLUDING ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR ANY PARTICULAR PURPOSE, NON-INFRINGEMENT ABCNode WARRANTIES OF
 * PERFORMANCE, ABCNode ANY WARRANTY THAT MIGHT OTHERWISE ARISE FROM COURSE OF
 * DEALING OR USAGE OF TRADE.  NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH
 * RESPECT TO THE USE OF THE SOFTWARE OR DOCUMENTATION. Under no circumstances
 * shall University be liable for incidental, special, indirect, direct or
 * consequential damages or loss of profits, interruption of business, or
 * related expenses which may arise from use of Software or Documentation,
 * including but not limited to those resulting from defects in Software and/or
 * Documentation, or loss or inaccuracy of data of any kind.
 *
 * Node for asynchrounous betweeness-centrality. 
 *
 * @author Dimitrios Prountzos <dprountz@cs.utexas.edu>
 */
#ifndef _BCNODE_H_
#define _BCNODE_H_

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "galois/substrate/SimpleLock.h"
#include "llvm/ADT/SmallVector.h"
#include "control.h"

extern int DEF_DISTANCE;

template <bool UseMarking=false, bool Concurrent=true>
struct BCNode {
  unsigned id;

  using LockType = typename std::conditional<Concurrent, 
                                             galois::substrate::SimpleLock,
                                             char>::type;
  LockType spinLock;

  //typedef std::vector<BCNode*> predTY;
  using predTY = llvm::SmallVector<BCNode*, 2>;
  predTY preds;

  unsigned distance;
  unsigned nsuccs;
  char mark;

  double sigma; 
  double delta;
  double bc;

  BCNode(const int _id)
    : id(_id), spinLock(), preds(), distance(DEF_DISTANCE), nsuccs(0), 
      sigma(0), delta(0), bc(0), mark(0) {}
  
  BCNode() 
    : id(DEF_DISTANCE), spinLock(), preds(), distance(DEF_DISTANCE), nsuccs(0),
      sigma(0), delta(0), bc(0), mark(0) {}
  
  /**
   * @param a Node to check if predecessor of this node
   * @returns true if node a is in predecessors of this node
   */
  bool predsContain(const BCNode* a) const {
    typename predTY::const_iterator it = preds.end();
    return (std::find(preds.begin(), preds.end(), a) != it); 
  }

  template<bool C = Concurrent, typename std::enable_if<C>::type* = nullptr>
  void lock() {
    spinLock.lock();
  }

  template<bool C = Concurrent, typename std::enable_if<C>::type* = nullptr>
  bool try_lock() {
    return spinLock.try_lock();
  }

  template<bool C = Concurrent, typename std::enable_if<C>::type* = nullptr>
  void unlock() {
    spinLock.unlock();
  }

  // below are no-ops for when concurrent is false
  template<bool C = Concurrent, typename std::enable_if<!C>::type* = nullptr>
  void lock() {
    // no-op
  }

  template<bool C = Concurrent, typename std::enable_if<!C>::type* = nullptr>
  bool try_lock() {
    return true;
  }

  /**
   * Return node as string.
   */
  std::string toString() const {
    std::ostringstream s;

    s << id << " distance: " << distance << " sigma: " << sigma << " bc: " 
      << bc << " nsuccs: " << nsuccs << " npreds: " << preds.size();

    return s.str();
  }

  /**
   * Reset everything but the BC value
   */
  void reset() {
    preds.clear();
    distance = DEF_DISTANCE;
    nsuccs = 0;
    sigma = 0;
    delta = 0;
    mark = 0;
  }

  /**
   * Sanity check to make sure node is reset
   */
  void checkClear() const {
    if (!preds.empty() || nsuccs != 0 || sigma != 0 || delta != 0)
      galois::gWarn("Problem, node not clear");

    assert(preds.empty());
    assert(distance == DEF_DISTANCE);
    assert(nsuccs == 0 && sigma == 0 && delta == 0);
  }  

  /**
   * Initialize this node as the source
   */
  void initAsSource() {
    distance = 0;
    sigma = 1;
  }

  /**
   * Mark this as 0.
   */
  template<bool M = UseMarking, typename std::enable_if<M>::type* = nullptr>
  void markOut() {
    if (Concurrent) {
    __sync_fetch_and_and(&mark, 0);
    } else {
      mark = 0;
    }
  }

  template<bool M = UseMarking, typename std::enable_if<!M>::type* = nullptr>
  void markOut() {
    // no-op
  }

  /**
   * @returns true if mark is set to 1
   */
  template<bool M = UseMarking, typename std::enable_if<M>::type* = nullptr>
  char isAlreadyIn() {
    if (Concurrent) {
      return __sync_fetch_and_or(&mark, 1);
    } else {
      char retval = mark;
      mark = 1;
      return retval;
    }
  }

  /**
   * @returns 0
   */
  template<bool M = UseMarking, typename std::enable_if<!M>::type* = nullptr>
  char isAlreadyIn() {
    return 0;
  }

};
#endif // _BCNODE_H_
