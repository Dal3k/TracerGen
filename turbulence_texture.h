//
// Created by Gaëtan Pusel on 01/05/2023.
//

#ifndef TRACERGEN_TURBULENCE_TEXTURE_H
#define TRACERGEN_TURBULENCE_TEXTURE_H

#include "texture.h"
#include "perlin.h"

class turbulence_texture : public texture {
public:
    turbulence_texture(double scale, color c0, color c1) : scale(scale), color0(c0), color1(c1) {}

    virtual color value(double u, double v, const point3 &p) const override {
        double noise_value = 0.5 * perlin_noise.turb(scale * p)
                             + 0.25 * perlin_noise.turb(2 * scale * p)
                             + 0.125 * perlin_noise.turb(4 * scale * p);
        return lerp(color0, color1, noise_value);
    }

private:
    perlin perlin_noise;
    double scale;
    color color0;
    color color1;

    color lerp(const color &c0, const color &c1, double t) const {
        return (1 - t) * c0 + t * c1;
    }
};


#endif //TRACERGEN_TURBULENCE_TEXTURE_H
