#if(CMAKE_COMPILER_IS_GNUCC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
#endif()

app(graphgrammar GraphGrammar.cpp model/Map.cpp Readers/SrtmReader.cpp libmgrs/mgrs.c libmgrs/polarst.c libmgrs/tranmerc.c libmgrs/utm.c libmgrs/ups.c utils/Utils.cpp)
app(graphgrammarTest test/TestMain.cpp)

#add_test_scale(small1 delaunayrefinement "${BASEINPUT}/reference/meshes/r10k.1")
#add_test_scale(small2 delaunayrefinement "${BASEINPUT}/meshes/250k.2")
#add_test_scale(web delaunayrefinement "${BASEINPUT}/meshes/r5M")
