#ifndef TERGEN_SRTMREADER_H
#define TERGEN_SRTMREADER_H

#include <cstdlib>
#include "../model/Map.h"

class SrtmReader {
private:
  static const int RESOLUTION = 3;

  static const unsigned short PIXEL_SIZE = 2;

  void read_from_multiple_files(const double west_border,
                                const double north_border,
                                const double east_border,
                                const double south_border, const char* map_dir,
                                double** map_data);

  void read_from_file(int north_border_int, int west_border_int, size_t rows,
                      size_t cols, int first_row, int first_col,
                      double** map_data, const char* map_dir);

  void skip_outliers(double* const* map_data, size_t length, size_t width);

  void get_filename(char* filename, const char* map_dir, int west_border_int,
                    int north_border_int);

  int border_to_int(const double border);

public:
  static const int VALUES_IN_DEGREE = 60 * 60 / RESOLUTION;

  Map* read(const double west_border, const double north_border,
            const double east_border, const double south_border,
            const char* map_dir);
};

#endif // TERGEN_SRTMREADER_H
