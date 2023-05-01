//
// Created by Gaëtan Pusel on 01/05/2023.
//

#ifndef TRACERGEN_WARPED_TEXTURE_H
#define TRACERGEN_WARPED_TEXTURE_H

#include "texture.h"
#include "perlin.h"

class warped_texture : public texture {
public:
    warped_texture(double scale) : scale(scale) {}

    virtual color value(double u, double v, const point3 &p) const override {
        vec3 scale_vec(scale, scale, scale);
        // Warp p using the formula f(p) = fbm(p+fbm(p+fbm(p)))
        point3 warped_p = p + scale_vec * fbm(scale_vec * (p + scale_vec * fbm(scale_vec * (p + scale_vec * fbm(scale_vec * p)))));

        // Create a grayscale value based on the warped position
        double warped_value = 0.5 * (1 + sin(warped_p.z()));

        return color(warped_value, warped_value, warped_value);
    }


private:
    perlin noise;
    double scale;

    double fbm(const point3 &p) const {
        return noise.turb(p);
    }
};

#endif //TRACERGEN_WARPED_TEXTURE_H
