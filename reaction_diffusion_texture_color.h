//
// Created by Gaëtan Pusel on 01/05/2023.
//

#ifndef TRACERGEN_REACTION_DIFFUSION_TEXTURE_COLOR_H
#define TRACERGEN_REACTION_DIFFUSION_TEXTURE_COLOR_H

#include "texture.h"
#include "perlin.h"

class reaction_diffusion_texture_color : public texture {
public:
    reaction_diffusion_texture_color(double scale, color c0, color c1) : scale(scale), color0(c0), color1(c1) {}

    virtual color value(double u, double v, const point3 &p) const override {
        double noise_value = perlin_noise.turb(scale * p);
        double rd_value = 0.5 * (1.0 + sin(scale * p.z() + 10.0 * noise_value));
        return lerp(color0, color1, rd_value);
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


#endif //TRACERGEN_REACTION_DIFFUSION_TEXTURE_COLOR_H
