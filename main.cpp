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
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"
#include "menger_sponge.h"
#include "tetrahedron.h"


struct image_settings {
    int image_height;
    int image_width;
    int samples_per_pixel;
    int max_depth;
    color background;
};

auto start_time = std::chrono::high_resolution_clock::now();

color ray_color(const ray &r, const color &background, const hittable &world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color(random_double(), random_double(), random_double()) *
                                  color(random_double(), random_double(), random_double());
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<moving_sphere>(
                            center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color(random_double(), random_double(), random_double());;
                    auto fuzz = random_double(0.1, 0.7);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}

hittable_list moon() {
    auto moon_texture = make_shared<image_texture>("moonmap.jpg");
    auto moon_surface = make_shared<lambertian>(moon_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, moon_surface);

    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    objects.add(box2);

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));


    return objects;
}

hittable_list cornell_smoke() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

    return objects;
}

hittable_list final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
            point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(random(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
                        make_shared<rotate_y>(
                                make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
                        vec3(-100, 270, 395)
                )
    );

    return objects;
}

hittable_list menger_sponge()
{
    hittable_list world;

    //auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    //world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    auto sponge_material = make_shared<lambertian>(color(0.2, 0.3, 0.5));
    world.add(make_shared<MengerSponge>(point3(0, 0, 1), 1.0, 4, sponge_material));
    //world.add(make_shared<tetrahedron>(point3(0, -0.5, 0), 1.0, 2, sponge_material));


    return world;
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

    const auto aspect_ratio = 1;

    const int image_height = 500;
    const int image_width = static_cast<int>(image_height * aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 5;
    //const int max_thread = 8;

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

    switch (9) {

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
