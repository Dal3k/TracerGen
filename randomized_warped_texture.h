//
// Created by Gaëtan Pusel on 02/05/2023.
//

#ifndef TRACERGEN_RANDOMIZED_WARPED_TEXTURE_H
#define TRACERGEN_RANDOMIZED_WARPED_TEXTURE_H

#include "texture.h"
#include "perlin.h"

class randomized_warped_texture : public texture {
public:
    randomized_warped_texture(double scale) : scale(scale) {
        base_color = color(random_double(), random_double(), random_double());
    }

    virtual color value(double u, double v, const point3 &p) const override {
        vec3 scale_vec(scale, scale, scale);
        point3 warped_p = p + scale_vec * fbm(scale_vec * (p + scale_vec * fbm(scale_vec * (p + scale_vec * fbm(scale_vec * p)))));

        double warped_value = 0.5 * (1 + sin(2 * M_PI * warped_p.z()));
        //double warped_value = 0.5 * (1 + sin(warped_p.z())); //petrol

        //return lerp(base_color, color(warped_value, warped_value, warped_value), warped_value); //petrol
        return lerp(base_color, color(warped_value, warped_value, warped_value), pow(warped_value, 2));
    }

private:
    perlin noise;
    double scale;
    color base_color;

    double fbm(const point3 &p) const {
        return noise.turb(p);
    }

    color lerp(const color &c0, const color &c1, double t) const {
        return (1 - t) * c0 + t * c1;
    }
};


#endif //TRACERGEN_RANDOMIZED_WARPED_TEXTURE_H
