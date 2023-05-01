#ifndef TRACERGEN_FRACTAL_NOISE_TEXTURE_H
#define TRACERGEN_FRACTAL_NOISE_TEXTURE_H

#include "texture.h"
#include "perlin.h"

class fractal_noise_texture : public texture {
public:
    fractal_noise_texture(double scale, int octaves, double persistence)
            : noise_generator(), scale(scale), octaves(octaves), persistence(persistence) {}

    virtual color value(double u, double v, const point3& p) const override {
        double total = 0;
        double frequency = 1;
        double amplitude = 1;
        double maxValue = 0;
        point3 scaled_p = p * scale;

        for (int i = 0; i < octaves; i++) {
            total += noise_generator.noise(scaled_p * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2;
        }

        double fractal_noise = total / maxValue;
        return color(fractal_noise, fractal_noise, fractal_noise);
    }

private:
    perlin noise_generator;
    double scale;
    int octaves;
    double persistence;
};

#endif // TRACERGEN_FRACTAL_NOISE_TEXTURE_H
