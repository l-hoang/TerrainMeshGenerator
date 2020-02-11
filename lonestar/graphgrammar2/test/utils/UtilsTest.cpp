#include "../../src/utils/Utils.h"
#include "../../src/model/Map.h"

TEST_CASE( "convertToUtm Test" ) {
    double longitude = 20.;
    double latitude = 50.;
    long zone = 34;
    char hemisphere = 'N';
    double northing = 5539109.82;
    double easting = 428333.55;
    double **placeholder = (double **)malloc(sizeof(double*));
    Map map{placeholder, 2, 2, 1., 1.};

    const std::pair<double, double> &pair = Utils::convertToUtm(latitude, longitude, map);
    
    REQUIRE(pair.first == easting);
    REQUIRE(pair.second == northing);
    REQUIRE(map.getZone() == zone);
    REQUIRE(map.getHemisphere() == hemisphere);
}