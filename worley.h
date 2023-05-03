#ifndef TRACERGEN_WORLEY_H
#define TRACERGEN_WORLEY_H

#include "utility.h"

class worley {
public:
    worley(int seed = 0) : seed(seed) {
        srand(seed);
    }

    double noise(const point3 &p, double cell_size = 1.0) const {
        double min_distance = std::numeric_limits<double>::max();
        vec3 cell_index = vec3(floor(p.x() / cell_size), floor(p.y() / cell_size), floor(p.z() / cell_size));

        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                for (int k = -1; k <= 1; ++k) {
                    vec3 neighbor_cell_index = cell_index + vec3(i, j, k);
                    vec3 random_offset = random_in_unit_cube(seed + hash(neighbor_cell_index));
                    vec3 cell_center = (neighbor_cell_index + random_offset) * cell_size;
                    vec3 cell_offset = p - cell_center;
                    double distance = cell_offset.length();
                    min_distance = std::min(min_distance, distance);
                }
            }
        }
        return min_distance / sqrt(3 * cell_size * cell_size); // Normalize to range [0, 1]
    }

private:
    int seed;

    static int hash(const vec3 &v) {
        int hx = std::hash<double>{}(v.x());
        int hy = std::hash<double>{}(v.y());
        int hz = std::hash<double>{}(v.z());
        return hx ^ (hy << 1) ^ (hz << 2);
    }

    vec3 random_in_unit_cube(int seed) const {
        srand(seed);
        return vec3(random_double(0, 1), random_double(0, 1), random_double(0, 1));
    }
};

#endif //TRACERGEN_WORLEY_H