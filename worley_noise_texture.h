// worley_noise_texture.h
#ifndef WORLEY_NOISE_TEXTURE_H
#define WORLEY_NOISE_TEXTURE_H

#include "utility.h"
#include "texture.h"
#include "vec3.h"
#include "perlin.h"

#include "utility.h"
#include "texture.h"
#include "vec3.h"
#include "perlin.h"

class worley_noise_texture : public texture {
public:
    worley_noise_texture() {}
    worley_noise_texture(double sc) : scale(sc) {}

    virtual color value(double u, double v, const point3& p) const override {
        return color(1.0, 1.0, 1.0) * (1.0 - worley_noise(scale * p));
    }

private:
    double worley_noise(const point3& p) const {
        double min_distance = 1e6;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
                    vec3 cell_offset(x, y, z);
                    point3 cell_point = p + cell_offset;
                    vec3 jitter = random_in_unit_sphere();
                    point3 jittered_point = cell_point + jitter;
                    vec3 difference = p - jittered_point;
                    double distance = dot(difference, difference);
                    min_distance = fmin(min_distance, distance);
                }
            }
        }
        return sqrt(min_distance);
    }

    double scale;
};

#endif
