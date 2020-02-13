#if(CMAKE_COMPILER_IS_GNUCC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
#endif()

app(graphgrammar src/GraphGrammar.cpp src/model/Map.cpp src/readers/SrtmReader.cpp src/readers/AsciiReader.cpp
        src/libmgrs/mgrs.c src/libmgrs/polarst.c src/libmgrs/tranmerc.c src/libmgrs/utm.c src/libmgrs/ups.c
        src/utils/Utils.cpp)
app(graphgrammarTest test/TestMain.cpp src/model/Map.cpp src/libmgrs/mgrs.c src/libmgrs/polarst.c src/libmgrs/tranmerc.c
        src/libmgrs/utm.c src/libmgrs/ups.c src/utils/Utils.cpp)

#add_test_scale(small1 delaunayrefinement "${BASEINPUT}/reference/meshes/r10k.1")
#add_test_scale(small2 delaunayrefinement "${BASEINPUT}/meshes/250k.2")
#add_test_scale(web delaunayrefinement "${BASEINPUT}/meshes/r5M")
