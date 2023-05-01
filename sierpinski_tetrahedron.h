//
// Created by Gaëtan Pusel on 01/05/2023.
//

#ifndef TRACERGEN_SIERPINSKI_TETRAHEDRON_H
#define TRACERGEN_SIERPINSKI_TETRAHEDRON_H

#include "hittable.h"
#include "hittable_list.h"
#include "triangle.h"
#include <memory>

class SierpinskiTetrahedron : public hittable {
public:
    SierpinskiTetrahedron(const point3& base, double side_length, int iterations, shared_ptr<material> mat);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

private:
    hittable_list tetrahedron_parts;
    void generate_tetrahedron(const point3& base, double side_length, int iterations, shared_ptr<material> mat);
};

SierpinskiTetrahedron::SierpinskiTetrahedron(const point3& base, double side_length, int iterations, shared_ptr<material> mat) {
    generate_tetrahedron(base, side_length, iterations, mat);
}

bool SierpinskiTetrahedron::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return tetrahedron_parts.hit(r, t_min, t_max, rec);
}

bool SierpinskiTetrahedron::bounding_box(double t0, double t1, aabb& output_box) const {
    return tetrahedron_parts.bounding_box(t0, t1, output_box);
}

void SierpinskiTetrahedron::generate_tetrahedron(const point3& base, double side_length, int iterations, shared_ptr<material> mat) {
    if (iterations == 0) {
        point3 A = base;
        point3 B = base + side_length * vec3(1, 0, 0);
        point3 C = base + side_length * vec3(0.5, sqrt(3) / 2, 0);
        point3 D = base + side_length * vec3(0.5, sqrt(3) / 6, sqrt(2.0 / 3));

        tetrahedron_parts.add(make_shared<triangle>(A, B, C, mat));
        tetrahedron_parts.add(make_shared<triangle>(A, C, D, mat));
        tetrahedron_parts.add(make_shared<triangle>(A, D, B, mat));
        tetrahedron_parts.add(make_shared<triangle>(B, D, C, mat));

        return;
    }

    generate_tetrahedron(base, side_length / 2, iterations - 1, mat);
    generate_tetrahedron(base + side_length / 2 * vec3(1, 0, 0), side_length / 2, iterations - 1, mat);
    generate_tetrahedron(base + side_length / 2 * vec3(0.5, sqrt(3) / 2, 0), side_length / 2, iterations - 1, mat);
    generate_tetrahedron(base + side_length / 2 * vec3(0.5, sqrt(3) / 6, sqrt(2.0 / 3)), side_length / 2, iterations - 1, mat);
}

#endif //TRACERGEN_SIERPINSKI_TETRAHEDRON_H
