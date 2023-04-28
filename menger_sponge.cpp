//
// Created by Gaëtan Pusel on 28/04/2023.
//

#include "menger_sponge.h"

MengerSponge::MengerSponge(const point3& center, double side_length, int iterations, shared_ptr<material> mat) {
    sponge = create_sponge(center, side_length, iterations, mat);
}

hittable_list MengerSponge::create_sponge(const point3& center, double side_length, int iterations, shared_ptr<material> mat) {
    hittable_list sponge_parts;
    if (iterations == 0) {
        sponge_parts.add(make_shared<box>(center - vec3(side_length / 2, side_length / 2, side_length / 2), center + vec3(side_length / 2, side_length / 2, side_length / 2), mat));
    } else {
        double new_side = side_length / 3;
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                for (int z = -1; z <= 1; ++z) {
                    if (x != 0 || y != 0 || z != 0) {
                        if (abs(x) + abs(y) + abs(z) != 3) {
                            point3 new_center = center + side_length * point3(x, y, z) / 3;
                            sponge_parts.add(make_shared<hittable_list>(create_sponge(new_center, new_side, iterations - 1, mat)));
                        }
                    }
                }
            }
        }
    }
    return sponge_parts;
}

bool MengerSponge::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return sponge.hit(r, t_min, t_max, rec);
}

bool MengerSponge::bounding_box(double t0, double t1, aabb& output_box) const {
    return sponge.bounding_box(t0, t1, output_box);
}
