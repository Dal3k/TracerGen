#ifndef WORLEY_TEXTURE_H
#define WORLEY_TEXTURE_H

#include "utility.h"
#include "texture.h"
#include "worley.h"

class worley_texture : public texture {
public:
    worley_texture(double cell_size = 1.0, int seed = 0)
            : worley_noise(seed), cell_size(cell_size) {}

    virtual color value(double u, double v, const point3& p) const override {
        double noise_value = worley_noise.noise(p, cell_size);
        return color(noise_value, noise_value, noise_value);
    }

private:
    worley worley_noise;
    double cell_size;
};

#endif // WORLEY_TEXTURE_H
