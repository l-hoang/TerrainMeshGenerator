#ifndef GALOIS_CONFIG_H
#define GALOIS_CONFIG_H


#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <cctype>

class Config {
public:
    double tolerance;
    bool version2D;
    int steps;

    Config() : tolerance(2), version2D(false), steps(10) {}

    static Config parse_arguments(int argc, char **argv) {
        Config config{};
        int argument;
        if (argc == 1) {
            fprintf(stderr, "%s", USAGE);
        }
        while ((argument = getopt(argc, argv, "t:23s:")) != -1)
            switch (argument)
            {
                case 't':
                    config.tolerance = atof(optarg);
                    break;
                case '2':
                    config.version2D = true;
                    break;
                case '3':
                    config.version2D = false;
                    break;
                case 's':
                    config.steps = atoi(optarg);
                    break;
                case '?':
                    if (optopt == 't' || optopt == 's' || optopt == '2' ) {

                        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                        fprintf(stderr, "%s", USAGE);
                    } else if (isprint (optopt)) {
                        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                        fprintf(stderr, "%s", USAGE);
                    } else {
                        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                        fprintf(stderr, "%s", USAGE);
                    }
                    exit(1);
                default:
                    abort();
            }
        return config;
    }

private:
    constexpr static char *const USAGE = "OPTIONS:\n"
                               "\t-t <tolerance>\n"
                               "\t-s <steps>\n"
                               "\t-2 turns on 2D version\n"
                               "\t-3 turns on 3D version\n"
                               "\n";
};


#endif //GALOIS_CONFIG_H
