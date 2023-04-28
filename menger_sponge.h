//
// Created by Gaëtan Pusel on 28/04/2023.
//

#ifndef MENGER_SPONGE_H
#define MENGER_SPONGE_H

#include "utility.h"
#include "hittable.h"
#include "hittable_list.h"
#include "box.h"
#include "material.h"
#include "vec3.h"

class MengerSponge : public hittable {
public:
    MengerSponge() {}
    MengerSponge(const point3& center, double side_length, int iterations, shared_ptr<material> mat);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

private:
    hittable_list create_sponge(const point3& center, double side_length, int iterations, shared_ptr<material> mat);
    hittable_list sponge;
};

#endif // MENGER_SPONGE_H

