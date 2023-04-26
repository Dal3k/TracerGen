#ifndef TRACERGEN_PYRAMID_H
#define TRACERGEN_PYRAMID_H

#include "hittable_list.h"
#include "triangle.h"
#include "vec3.h"

class pyramid : public hittable {
public:
    pyramid() {}

    pyramid(const point3& base_center, double height, double base_side_length, shared_ptr<material> mat)
            : mat_ptr(mat) {
        point3 a = base_center + vec3(-base_side_length / 2, 0, -base_side_length / 2);
        point3 b = base_center + vec3(-base_side_length / 2, 0, base_side_length / 2);
        point3 c = base_center + vec3(base_side_length / 2, 0, base_side_length / 2);
        point3 d = base_center + vec3(base_side_length / 2, 0, -base_side_length / 2);
        point3 apex = base_center + vec3(0, height, 0);

        sides.add(make_shared<triangle>(a, b, apex, mat));
        sides.add(make_shared<triangle>(b, c, apex, mat));
        sides.add(make_shared<triangle>(c, d, apex, mat));
        sides.add(make_shared<triangle>(d, a, apex, mat));
    }

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        return sides.hit(r, t_min, t_max, rec);
    }

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        return sides.bounding_box(time0, time1, output_box);
    }

public:
    hittable_list sides;
    shared_ptr<material> mat_ptr;
};

#endif // TRACERGEN_PYRAMID_H
