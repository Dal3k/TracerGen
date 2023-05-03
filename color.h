#ifndef TRACERGEN_COLOR_H
#define TRACERGEN_COLOR_H

#include "vec3.h"

#include <iostream>

void write_color(std::vector<unsigned char> &image_data, int index, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma-correct for gamma = 2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    std::cout << "Color values before clamping: " << r << " " << g << " " << b << std::endl;


    // Write the translated [0,255] value of each color component.
    image_data[index * 3 + 0] = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    image_data[index * 3 + 1] = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    image_data[index * 3 + 2] = static_cast<int>(256 * clamp(b, 0.0, 0.999));
}

#endif //TRACERGEN_COLOR_H