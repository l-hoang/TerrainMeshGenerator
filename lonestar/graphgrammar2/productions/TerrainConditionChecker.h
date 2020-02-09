#ifndef GALOIS_TERRAINCONDITIONCHECKER_H
#define GALOIS_TERRAINCONDITIONCHECKER_H

#include <values.h>
#include <cmath>
#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"
#include "../model/Map.h"
#include "../model/ProductionState.h"
#include "../libmgrs/utm.h"

class TerrainConditionChecker {
public:
    explicit TerrainConditionChecker(Map &map) : map(map) {}

    bool execute(GNode &node, double tolerance, ConnectivityManager connManager) {

        NodeData &nodeData = node->getData();
        if (!checkBasicApplicabilityCondition(nodeData)) {
            return false;
        }

        vector<Coordinates> verticesCoords = connManager.getVerticesCoords(node);

        if (!inside_condition(verticesCoords, tolerance)) {
            return false;
        }

        nodeData.setToRefine(true);
        return true;
    }

private:
    Map &map;

    bool checkBasicApplicabilityCondition(const NodeData &nodeData) const {
        return nodeData.isHyperEdge();
    }

    bool inside_condition(const vector<Coordinates> &verticesCoords, double tolerance) {
        double lowest_x =
                verticesCoords[0].getX() < verticesCoords[1].getX()
                ? verticesCoords[0].getX() : verticesCoords[1].getX();
        lowest_x = verticesCoords[2].getX() < lowest_x
                   ? verticesCoords[2].getX() : lowest_x;
        double highest_x =
                verticesCoords[0].getX() > verticesCoords[1].getX()
                ? verticesCoords[0].getX() : verticesCoords[1].getX();
        highest_x = verticesCoords[2].getX() > highest_x
                    ? verticesCoords[2].getX() : highest_x;
        double lowest_y =
                verticesCoords[0].getY() < verticesCoords[1].getY()
                ? verticesCoords[0].getY() : verticesCoords[1].getY();
        lowest_y = verticesCoords[2].getY() < lowest_y
                   ? verticesCoords[2].getY() : lowest_y;
        double highest_y =
                verticesCoords[0].getY() > verticesCoords[1].getY()
                ? verticesCoords[0].getY() : verticesCoords[1].getY();
        highest_y = verticesCoords[2].getY() > highest_y
                    ? verticesCoords[2].getY() : highest_y;

        double step = map.isUtm() ? 90 : map.getCellWidth();
        for (double i = lowest_x; i <= highest_x; i += step) {
            for (double j = lowest_y; j <= highest_y; j += step) {
                Coordinates tmp{i, j, 0.};
//                struct point *tmp = (struct point *) malloc(sizeof(struct point));
//                tmp->x = i;
//                tmp->y = j;
                double barycentric_point[3];
                compute_barycentric_coords(barycentric_point, tmp, verticesCoords);
               if (is_inside_triangle(barycentric_point)) {
                    double height = 0;
                    for (int k = 0; k < 3; ++k) {
                        height += barycentric_point[k] * verticesCoords[k].getZ();
                    }
                    if (fabs(height - get_height(i, j)) > tolerance) {
                        return true;
                    }
                }
//                free(tmp);
            }
        }
        return false;
    }

    void
    compute_barycentric_coords(double *barycentric_coords, Coordinates &point, const vector<Coordinates>& verticesCoords) {
        double triangle_area = get_area(verticesCoords[0], verticesCoords[1], verticesCoords[2]);
        barycentric_coords[2] = get_area(point, verticesCoords[0], verticesCoords[1]) / triangle_area;
        barycentric_coords[1] = get_area(point, verticesCoords[2], verticesCoords[0]) / triangle_area;
        barycentric_coords[0] = get_area(point, verticesCoords[1], verticesCoords[2]) / triangle_area;
    }

    bool
    is_inside_triangle(double barycentric_coords[]) {
        return !greater(barycentric_coords[0] + barycentric_coords[1] + barycentric_coords[2], 1.);
    }

    double
    get_height(double lon, double lat)
    {
        double x, y;
        //convert to geodetic if required
        if (map.isUtm()) {
            if (Convert_UTM_To_Geodetic(map.getZone(), map.getHemisphere(), lon, lat, &y, &x)) {
                fprintf(stderr, "Error during conversion.\n");
                exit(18);
            }
            x = r2d(x);
            y = r2d(y);
        } else {
            x = lon;
            y = lat;
        }
        //using bilinear interpolation
        double top_left = get_height_wo_interpol(x, y, 1);
        double top_right = get_height_wo_interpol(x, y, 2);
        double bottom_right = get_height_wo_interpol(x, y, 3);
        double bottom_left = get_height_wo_interpol(x, y, 4);;

        double x_fract = (x - map.getWestBorder()) / map.getCellWidth() -
                         floor2((x - map.getWestBorder()) / map.getCellWidth() );
        double y_fract = fabs(y - map.getNorthBorder()) / map.getCellLength() -
                         floor2(fabs(y - map.getNorthBorder()) / map.getCellLength() );

        double height = 0.;
        height += top_left * (1 - x_fract) * (1 - y_fract);
        height += top_right * x_fract * (1 - y_fract);
        height += bottom_right * x_fract * y_fract;
        height += bottom_left * (1 - x_fract) * y_fract;

        return height;
    }

    //corner: 1 - top_left, 2 - top_right, 3 - bottom_right, 4 - bottom_left
    double
    get_height_wo_interpol(double lon, double lat, int corner)
    {
        double (*fun1)(double);
        double (*fun2)(double);
        switch (corner) {
            case 1:
                fun1 = floor2;
                fun2 = floor2;
                break;
            case 2:
                fun1 = floor2;
                fun2 = ceil2;
                break;
            case 3:
                fun1 = ceil2;
                fun2 = ceil2;
                break;
            case 4:
                fun1 = ceil2;
                fun2 = floor2;
                break;
            default:
                return MINDOUBLE;
        }

        int y, x;
        if (fun1((map.getNorthBorder() - lat) / map.getCellLength()) > map.getLength() - 1) {
            y = (int) (map.getLength() - 1);
        } else if (fun1((map.getNorthBorder() - lat) / map.getCellLength()) < 0) {
            y = 0;
        } else {
            y = (int) fun1((map.getNorthBorder() - lat) / map.getCellLength());
        }
        if (fun2((lon - map.getWestBorder()) / map.getCellWidth()) > map.getWidth() - 1) {
            x = (int) (map.getWidth() - 1);
        } else if (fun2((lon - map.getWestBorder()) / map.getCellWidth()) < 0) {
            x = 0;
        } else {
            x = (int) fun2((lon - map.getWestBorder()) / map.getCellWidth());
        }
        return map.getData()[y][x];
    }

    double
    get_area(const Coordinates &a, const Coordinates &b, const Coordinates &c)
    {
        return 0.5 * fabs((b.getX() - a.getX()) * (c.getY() - a.getY()) - (b.getY() - a.getY()) * (c.getX() - a.getX()));
    }

    double
    r2d(double radians)
    {
        return radians * 180 / M_PI;
    }

    static double
    floor2(double a)
    {
        double b = (int) a;
        if (!(!greater(b, a) && greater(b+1, a))) {
            ++b;
        }
        return b;
    }

    static double
    ceil2(double a)
    {
        return floor2(a) + 1;
    }

};


#endif //GALOIS_TERRAINCONDITIONCHECKER_H
