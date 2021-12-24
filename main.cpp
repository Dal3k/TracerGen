#include <iostream>
#include <fstream>
#include <thread>

#include "utility.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"

struct image_settings {
    int image_height;
    int image_width;
    int samples_per_pixel;
    int max_depth;
};


color ray_color(const ray &r, const hittable &world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
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


void worker(struct image_settings &settings, const std::shared_ptr<std::vector<color>>& image, int max_thread,
            int thread, camera &cam, hittable_list &world) {
    for (int j = thread; j < settings.image_height; j += max_thread) {
        for (int i = 0; i < settings.image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < settings.samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (settings.image_width - 1);
                auto v = (j + random_double()) / (settings.image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, settings.max_depth);
            }
            (*image)[j * settings.image_width + i] = pixel_color;
        }
    }
}


int main() {
    // Image

    const auto aspect_ratio = 16.0 / 9.0;
    const int image_height = 1080;
    const int image_width = static_cast<int>(image_height * aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 5;
    const int max_thread = 8;

    std::ofstream myfile;
    myfile.open("image.ppm");
    auto image = std::make_shared<std::vector<color>>(image_height * image_width);
    std::vector<std::thread> threads;

    struct image_settings settings = {image_height, image_width, samples_per_pixel, max_depth};

    // World

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;

    switch (1) {
        case 1:
            world = random_scene();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;
        case 2:
            world = two_spheres();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        case 3:
            world = two_perlin_spheres();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
        default:
        case 4:
            world = moon();
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;
    }

    // Camera

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render

    myfile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    threads.reserve(max_thread);
    for (int i = 0; i < max_thread; ++i) {
        threads.emplace_back(worker, std::ref(settings), std::ref(image), max_thread, i, std::ref(cam), std::ref(world));
    }

    for (auto &thread: threads) {
        thread.join();
    }

    for (int i = image_height - 1; i >= 0; i--) {
        for (int j = 0; j < image_width; ++j) {
            write_color(myfile, (*image)[i * image_width + j], samples_per_pixel);
        }
    }

    std::cout << "\nDone!\n";
    myfile.close();
    return 0;
}
