//
// Created by Gaëtan Pusel on 01/05/2023.
//

#ifndef TRACERGEN_SCENES_H
#define TRACERGEN_SCENES_H

#pragma once

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
#include "fractal_tree_3d.h"
#include "cylinder.h"
#include "barnsley_fern.h"
#include "sierpinski_tetrahedron.h"
#include "fractal_noise_texture.h"
#include "worley_noise_texture.h"
#include "reaction_diffusion_texture.h"
#include "reaction_diffusion_texture_color.h"
#include "turbulence_texture.h"
#include "warped_texture.h"
#include "randomized_warped_texture.h"
#include "procedural_warped_texture.h"
#include "full_procedural_warped_texture.h"

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

hittable_list create_fractal_tree_scene() {
    hittable_list objects;

    auto material1 = make_shared<lambertian>(color(0.2, 0.8, 0.2));

    // First tree configuration
    double initial_length1 = 1.0;
    double initial_radius1 = 0.1;
    int depth1 = 4;
    FractalTree3D tree1(point3(0, 0, 0), initial_length1, initial_radius1, depth1, material1);
    objects.add(make_shared<FractalTree3D>(tree1));

    // Second tree configuration
    double initial_length2 = 1.5;
    double initial_radius2 = 0.15;
    int depth2 = 5;
    FractalTree3D tree2(point3(5, 0, 0), initial_length2, initial_radius2, depth2, material1);
    objects.add(make_shared<FractalTree3D>(tree2));

    // Third tree configuration
    double initial_length3 = 2.0;
    double initial_radius3 = 0.2;
    int depth3 = 3;
    FractalTree3D tree3(point3(-5, 0, 0), initial_length3, initial_radius3, depth3, material1);
    objects.add(make_shared<FractalTree3D>(tree3));

    return objects;
}

hittable_list create_forest() {
    hittable_list forest;
    auto tree_material = make_shared<lambertian>(color(0.4, 0.2, 0.1));

    int num_trees = 20;
    double spacing = 10.0;

    for (int i = 0; i < num_trees; ++i) {
        for (int j = 0; j < num_trees; ++j) {
            double initial_length = random_double(4.0, 6.0);
            double initial_radius = initial_length / 20.0;
            int iterations = random_int(2, 4);

            point3 root(i * spacing, 0, j * spacing);
            forest.add(make_shared<FractalTree3D>(root, initial_length, initial_radius, iterations, tree_material));
        }
    }

    return forest;
}

hittable_list create_ferne() {
    hittable_list world;
    auto fern_material = make_shared<lambertian>(color(0.1, 0.8, 0.1));
    auto fern = make_shared<BarnsleyFern>(50000, 10, fern_material);
    world.add(fern);
    return world;
}

hittable_list sierpinski() {
    hittable_list objects;

    auto mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    int depth = 6;
    point3 center(0, 0, 0);
    double side_length = 6.0;

    objects = SierpinskiTetrahedron::create(depth, center, side_length, mat);

    return objects;
}

hittable_list fractal_noise_scene() {
    hittable_list world;

    auto worley_noise_texture_ptr = make_shared<worley_noise_texture>(5.0);
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, make_shared<lambertian>(worley_noise_texture_ptr)));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, make_shared<lambertian>(worley_noise_texture_ptr)));
    return world;
}

hittable_list reaction_diffusion_scene() {
    hittable_list objects;

    auto reaction_diffusion_tex = make_shared<reaction_diffusion_texture>(2.0);
    auto reaction_diffusion_mat = make_shared<lambertian>(reaction_diffusion_tex);

    objects.add(make_shared<sphere>(point3(0, 0, 0), 50, reaction_diffusion_mat));

    return objects;
}

hittable_list reaction_diffusion_color_scene() {
    hittable_list objects;

    auto rd_texture = make_shared<reaction_diffusion_texture_color>(5.0, color(0.2, 0.3, 0.6), color(0.8, 0.6, 0.2));
    auto rd_material = make_shared<lambertian>(rd_texture);

    objects.add(make_shared<sphere>(point3(0, 0, 0), 100, rd_material));

    return objects;
}

hittable_list turbulence_scene() {
    hittable_list objects;

    auto turbulence_tex = make_shared<turbulence_texture>(1.0, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto turbulence_mat = make_shared<lambertian>(turbulence_tex);

    objects.add(make_shared<sphere>(point3(0, 0, 0), 100, turbulence_mat));

    return objects;
}

hittable_list warped_scene() {
    hittable_list objects;

    auto warped_tex = make_shared<warped_texture>(1.0);
    auto warped_mat = make_shared<lambertian>(warped_tex);

    objects.add(make_shared<sphere>(point3(0, 0, 0), 100, warped_mat));

    return objects;
}

hittable_list random_warped_scene_color() {
    hittable_list objects;

    auto warped_tex = make_shared<warped_texture>(1.0);
    auto warped_mat = make_shared<lambertian>(warped_tex);

    objects.add(make_shared<sphere>(point3(0, 0, 0), 100, warped_mat));

    return objects;
}

hittable_list procedural_warped_scene_color() {
    hittable_list objects;

    auto warped_tex = make_shared<procedural_warped_texture>(1.5);
    auto warped_mat = make_shared<lambertian>(warped_tex);

    objects.add(make_shared<sphere>(point3(0, 0, 0), 100, warped_mat));

    return objects;
}

hittable_list full_procedural_warped_scene() {
    hittable_list objects;

    auto procedural_warped_tex = make_shared<full_procedural_warped_texture>();
    auto procedural_warped_mat = make_shared<lambertian>(procedural_warped_tex);

    objects.add(make_shared<sphere>(point3(0, 0, 0), 100, procedural_warped_mat));

    return objects;
}







#endif //TRACERGEN_SCENES_H
