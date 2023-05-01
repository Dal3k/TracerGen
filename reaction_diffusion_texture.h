#ifndef TRACERGEN_REACTION_DIFFUSION_TEXTURE_H
#define TRACERGEN_REACTION_DIFFUSION_TEXTURE_H

#include "texture.h"
#include "perlin.h"
#include "utility.h"

class reaction_diffusion_texture : public texture {
public:
    reaction_diffusion_texture(double scale)
            : scale(scale) {}

    color value(double u, double v, const point3 &p) const override {
        double noise_value = perlin_noise.turb(scale * p);
        double sine_value = 0.5 * (1 + sin(scale * p.z() + 10 * noise_value));
        return sine_value * color(1, 1, 1);
    }

private:
    perlin perlin_noise;
    double scale;
};

#endif // TRACERGEN_REACTION_DIFFUSION_TEXTURE_H
