#include "../../src/utils/Utils.h"
#include "../../src/model/Map.h"

TEST_CASE( "Map::get_height Test" ) {
    double value = 45.;
    double **placeholder = (double **)malloc(sizeof(double*));
    placeholder[0] = (double *)malloc(2*sizeof(double));
    placeholder[0][0] = value + 8;
    placeholder[0][1] = value;
    Map map{placeholder, 2, 1, 1., 1.};


    REQUIRE(fabs(map.get_height(1, 0, false) - value) < 1e-1);
}

TEST_CASE( "Map::get_height Test2" ) {
    double value = 45.;
    double north_border = 49;
    double west_border = 20;
    double **placeholder = (double **)malloc(sizeof(double*));
    placeholder[0] = (double *)malloc(2*sizeof(double));
    placeholder[0][0] = value + 8;
    placeholder[0][1] = value;

    Map map{placeholder, 2, 1, 0.5, 0.5};
    map.setNorthBorder(north_border);
    map.setWestBorder(west_border);

    REQUIRE(fabs(map.get_height(west_border + 0.5, north_border, false) - value) < 1e-1);
}