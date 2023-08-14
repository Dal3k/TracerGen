#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/tbb.h>


#include "utility.h"

#include "aarect.h"
#include "color.h"
#include "hittable_list.h"
#include "camera.h"
#include "scenes.h"
#include "blackhole.h"

struct image_settings {
    int image_height;
    int image_width;
    int samples_per_pixel;
    int max_depth;
    color background;
};

auto start_time = std::chrono::high_resolution_clock::now();

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec)) {
        return background;
    }

    // Check if the hit object is a Blackhole
    if(auto* bh = dynamic_cast<Blackhole*>(rec.mat_ptr.get())) {
        // Modify the ray using Blackhole::interact()
        ray new_ray = bh->interact(r, rec);
        return 0.5 * ray_color(new_ray, background, world, depth-1);
    }

    color attenuation;
    ray scattered;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, background, world, depth-1);
    return color(0, 0, 0);
}



void print_formatted_time(std::ostream& os, int seconds) {
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;

    if (hours > 0) {
        os << hours << "h ";
    }
    if (minutes > 0) {
        os << minutes << "m ";
    }
    os << seconds << "s";
}

void print_progress(double progress, const std::chrono::high_resolution_clock::time_point &start_time, int total_pixels, int image_width, int image_height) {
    int bar_width = 50;

    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration<double>(current_time - start_time).count();
    auto estimated_remaining_time = elapsed_time * (1.0 - progress) / progress;

    std::cout << "[";
    int pos = static_cast<int>(bar_width * progress);
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(progress * 100.0) << " %"
              << " (" << total_pixels << " of " << image_height * image_width << " pixels)"
              << " Elapsed: ";
    print_formatted_time(std::cout, static_cast<int>(elapsed_time));
    std::cout << " Remaining: ";
    print_formatted_time(std::cout, static_cast<int>(estimated_remaining_time));
    std::cout << "    \r";
    std::cout.flush();
}


std::mutex progress_mutex;

void render_tile(const tbb::blocked_range2d<int>& tile_range, struct image_settings &settings, const std::shared_ptr<std::vector<color>> &image,
                 camera &cam, hittable_list &world, std::atomic<int> &lines_rendered) {
    for (int j = tile_range.rows().begin(); j != tile_range.rows().end(); ++j) {
        for (int i = tile_range.cols().begin(); i != tile_range.cols().end(); ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < settings.samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (settings.image_width - 1);
                auto v = (j + random_double()) / (settings.image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, settings.background, world, settings.max_depth);
            }
            (*image)[j * settings.image_width + i] = pixel_color;
        }
    }

    int pixels_rendered_in_tile = (tile_range.rows().end() - tile_range.rows().begin()) * (tile_range.cols().end() - tile_range.cols().begin());

    {
        std::lock_guard<std::mutex> lock(progress_mutex);
        lines_rendered += pixels_rendered_in_tile;
        double progress = static_cast<double>(lines_rendered) / (settings.image_height * settings.image_width);
        print_progress(progress, start_time, lines_rendered, settings.image_width, settings.image_height);
    }
}



int main() {
    // Image

    const auto aspect_ratio = 0.5;

    const int image_height = 2000;
    const int image_width = static_cast<int>(image_height * aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 5;

    std::atomic<int> lines_rendered(0);

    auto image = std::make_shared<std::vector<color>>(image_height * image_width);
    std::vector<std::thread> threads;
    color back = color(0, 0, 0);
    struct image_settings settings = {image_height, image_width, samples_per_pixel, max_depth, back};

    // World

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0, 0, 0);

    switch (14) {

        case 1:
            world = random_scene();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;
        case 2:
            world = two_spheres();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 3:
            world = two_perlin_spheres();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 4:
            world = moon();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 5:
            world = simple_light();
            settings.background = color(0, 0, 0);
            lookfrom = point3(26, 3, 6);
            lookat = point3(0, 2, 0);
            vfov = 20.0;
            break;
        case 6:
            world = cornell_box();
            background = color(0, 0, 0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        case 7:
            world = cornell_smoke();
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        case 8:
            world = final_scene();
            background = color(0, 0, 0);
            lookfrom = point3(478, 278, -600);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        default:
        case 9:
            world = menger_sponge();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(0, 0, 3);
            lookat = point3(0, 0, 0);
            vfov = 40.0;
            break;
        case 10:
            world = create_fractal_tree_scene();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(3, 3, 10); // Position the camera at a slightly elevated angle and some distance away
            lookat = point3(0, 1, 0);    // Aim the camera at the base of the first tree
            vfov = 40.0;
            break;
        case 11:
            world = create_forest();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(100, 50, 100); // Position the camera at a slightly elevated angle and some distance away
            lookat = point3(100, 5, 50);    // Aim the camera at the base of the first tree
            vfov = 40.0;
            break;
        case 12:
            world = create_ferne();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(0, 100, 150); // Position the camera at a slightly elevated angle and some distance away
            lookat = point3(0, 50, 0);    // Aim the camera at the base of the first tree
            vfov = 40.0;
            break;
        case 13:
            world = sierpinski();
            settings.background = color(0.70, 0.80, 1.00);
            lookfrom = point3(0, 0, 15);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 14:
            world = blackhole_scene();
            settings.background = color(0, 0, 0); // Set background to black
            lookfrom = point3(50, 50, 50);
            lookat = point3(0, 0, 0);
            break;

    }

    // Camera

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

    // Set the desired tile size
    int desired_tile_size = 32;

    // Calculate the number of horizontal and vertical tiles
    int num_horizontal_tiles = (image_width + desired_tile_size - 1) / desired_tile_size;
    int num_vertical_tiles = (image_height + desired_tile_size - 1) / desired_tile_size;

    // Calculate the actual tile size based on the number of tiles
    int actual_tile_width = (image_width + num_horizontal_tiles - 1) / num_horizontal_tiles;
    int actual_tile_height = (image_height + num_vertical_tiles - 1) / num_vertical_tiles;

    tbb::parallel_for(
            tbb::blocked_range2d<int>(0, image_height, actual_tile_height, 0, image_width, actual_tile_width),
            [&](const tbb::blocked_range2d<int>& tile_range) {
                render_tile(tile_range, settings, image, cam, world, lines_rendered);
            }
    );


    std::vector<unsigned char> image_data(image_width * image_height * 3);

    for (int i = image_height - 1; i >= 0; i--) {
        for (int j = 0; j < image_width; ++j) {
            int index = (image_height - i - 1) * image_width + j;
            write_color(image_data, index, (*image)[i * image_width + j], samples_per_pixel);
        }
    }

    std::cout << "\nDone!\n";
    stbi_write_png("image.png", image_width, image_height, 3, image_data.data(), image_width * 3);
    return 0;
}
