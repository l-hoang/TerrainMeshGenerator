#include <cstdio>
#include <values.h>
#include <cmath>
#include "Map.h"
#include "../utils/Utils.h"
#include "../libmgrs/utm.h"

double **Map::init_map_data(size_t rows, size_t cols) {
    double **map;
    map = (double **)malloc(rows * sizeof(double *));
    for (size_t i = 0; i < rows; ++i) {
        map[i] = (double *)malloc(cols * sizeof(double));
    }
    return map;
}

void Map::print_map() {
    for (int i = 0; i < this->length; ++i) {
        for (int j = 0; j < this->width; ++j) {
            fprintf(stdout, "%5.0lf ", this->data[i][j]);
        }
        fprintf(stdout, "\n");
    }
}

double Map::get_height(double lon, double lat) {
    fprintf(stderr, "DUPA 4\n");
    return get_height(lon, lat, utm);
    fprintf(stderr, "DUPA 5\n");
}

double Map::get_height(double lon, double lat, bool convert) {
    double x, y;
    //convert to geodetic if required
    if (convert) {
        if (Convert_UTM_To_Geodetic(zone, hemisphere, lon, lat, &y, &x)) {
            fprintf(stderr, "Error during conversion to geodetic.\n");
            exit(18);
        }
        x = Utils::r2d(x);
        y = Utils::r2d(y);
    } else {
        x = lon;
        y = lat;
    }
    //using bilinear interpolation
    double top_left = get_height_wo_interpol(x, y, 1);
    double top_right = get_height_wo_interpol(x, y, 2);
    double bottom_right = get_height_wo_interpol(x, y, 3);
    double bottom_left = get_height_wo_interpol(x, y, 4);

    double x_fract = (x - west_border) / cell_width -
                     Utils::floor2((x - west_border) / cell_width );
    double y_fract = fabs(y - north_border) / cell_length -
                     Utils::floor2(fabs(y - north_border) / cell_length );

    double height = 0.;
    height += top_left * (1 - x_fract) * (1 - y_fract);
    height += top_right * x_fract * (1 - y_fract);
    height += bottom_right * x_fract * y_fract;
    height += bottom_left * (1 - x_fract) * y_fract;

    return height;
}

//corner: 1 - top_left, 2 - top_right, 3 - bottom_right, 4 - bottom_left
double Map::get_height_wo_interpol(double lon, double lat, int corner) {
    double (*fun1)(double);
    double (*fun2)(double);
    switch (corner) {
        case 1:
            fun1 = Utils::floor2;
            fun2 = Utils::floor2;
            break;
        case 2:
            fun1 = Utils::floor2;
            fun2 = Utils::ceil2;
            break;
        case 3:
            fun1 = Utils::ceil2;
            fun2 = Utils::ceil2;
            break;
        case 4:
            fun1 = Utils::ceil2;
            fun2 = Utils::floor2;
            break;
        default:
            return MINDOUBLE;
    }

    int y, x;
    if (fun1((north_border - lat) / cell_length) > length - 1) {
        y = (int) (length - 1);
    } else if (fun1((north_border - lat) / cell_length) < 0) {
        y = 0;
    } else {
        y = (int) fun1((north_border - lat) / cell_length);
    }
    if (fun2((lon - west_border) / cell_width) > width - 1) {
        x = (int) (width - 1);
    } else if (fun2((lon - west_border) / cell_width) < 0) {
        x = 0;
    } else {
        x = (int) fun2((lon - west_border) / cell_width);
    }
    return data[y][x];
}


Map::~Map() {
    for (size_t i = 0; i < this->length; ++i) {
        free((double *) this->data[i]);
    }
    free(this->data);
}
