//
// Created by Gaëtan Pusel on 02/05/2023.
//

#ifndef TRACERGEN_FULL_PROCEDURAL_WARPED_TEXTURE_H
#define TRACERGEN_FULL_PROCEDURAL_WARPED_TEXTURE_H

#include <vector>
#include <functional>
#include "texture.h"
#include "perlin.h"

enum class NoiseFunction {
    PerlinNoise,
    WorleyNoise
};

enum class Operation {
    Add,
    Subtract,
    Multiply
};

class full_procedural_warped_texture : public texture {
public:
    full_procedural_warped_texture() {
        // Step 1: Define a set of noise functions and mathematical operations
        noise_functions = {NoiseFunction::PerlinNoise, NoiseFunction::WorleyNoise};
        operations = {Operation::Add, Operation::Subtract, Operation::Multiply};

        // Step 2: Define a set of rules for combining functions and operations
        // In this case, we'll simply randomize the selections

        // Randomly select a noise function and operation for each level
        for (int i = 0; i < 3; ++i) {
            selected_noise_functions.push_back(noise_functions[random_int(0, noise_functions.size() - 1)]);
            selected_operations.push_back(operations[random_int(0, operations.size() - 1)]);
        }
    }

    virtual color value(double u, double v, const point3 &p) const override {
        double worley_value = worley_noise(p);
        std::cout << "Worley value: " << worley_value << std::endl;

        color col = color(worley_value, worley_value, worley_value);
        std::cout << "Color: " << col.x() << " " << col.y() << " " << col.z() << std::endl;
        return col;
        /**
        point3 warped_p = p;
        double q = 0.0;
        double r = 0.0;

        // Step 3: Randomly select or procedurally generate a combination of noise functions and operations
        for (int i = 0; i < selected_noise_functions.size(); ++i) {
            double noise_value = apply_noise_function(warped_p, selected_noise_functions[i]);
            q += noise_value;
            r += 1.0 - noise_value;
            warped_p = apply_operation(warped_p, point3(noise_value, noise_value, noise_value), selected_operations[i]);
        }

        // Normalize q and r values to the range [0, 1]
        q /= static_cast<double>(selected_noise_functions.size());
        r /= static_cast<double>(selected_noise_functions.size());

        // Step 4: Create the final texture with colors based on q and r
        double warped_value = 0.5 * (1 + sin(2 * M_PI * warped_p.z()));
        return color(warped_value * q, warped_value * r, warped_value);
         **/
    }




private:
    perlin perlin_noise_gen;
    double scale;
    std::vector<NoiseFunction> noise_functions;
    std::vector<Operation> operations;

    std::vector<NoiseFunction> selected_noise_functions;
    std::vector<Operation> selected_operations;



    double perlin_noise(const point3 &p) const {
        return perlin_noise_gen.noise(p);
    }

    vec3 hash(const vec3 &v) const {
        double x = sin(v.x() * 127.1 + v.y() * 311.7 + v.z() * 419.3) * 43758.5453;
        double y = sin(v.x() * 269.5 + v.y() * 183.3 + v.z() * 372.9) * 43758.5453;
        double z = sin(v.x() * 419.7 + v.y() * 371.9 + v.z() * 139.1) * 43758.5453;

        double fractional_x = x - floor(x);
        double fractional_y = y - floor(y);
        double fractional_z = z - floor(z);

        return vec3(fractional_x, fractional_y, fractional_z);
    }

    double worley_noise(const point3 &p) const {
        double min_distance = std::numeric_limits<double>::max();
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                for (int k = -1; k <= 1; ++k) {
                    vec3 cell_index = vec3(i, j, k);
                    vec3 random_offset = hash(cell_index);
                    vec3 cell_center = cell_index + random_offset;
                    vec3 cell_offset = p - cell_center;
                    double distance = cell_offset.length_squared(); // Use squared distance to avoid the sqrt operation
                    min_distance = std::min(min_distance, distance);
                }
            }
        }
        return sqrt(min_distance); // Return the square root of the minimum squared distance
    }



    double apply_noise_function(const point3 &p, NoiseFunction noise_function) const {
        switch (noise_function) {
            case NoiseFunction::PerlinNoise:
                return perlin_noise(p);
            case NoiseFunction::WorleyNoise:
                return worley_noise(p);
            default:
                return 0.0;
        }
    }

    point3 apply_operation(const point3 &a, const point3 &b, Operation operation) const {
        switch (operation) {
            case Operation::Add:
                return a + b;
            case Operation::Subtract:
                return a - b;
            case Operation::Multiply:
                return a * b;
            default:
                return point3(0.0, 0.0, 0.0);
        }
    }
};

#endif //TRACERGEN_FULL_PROCEDURAL_WARPED_TEXTURE_H

