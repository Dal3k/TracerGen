//
// Created by Gaëtan Pusel on 01/05/2023.
//

#ifndef TRACERGEN_SIERPINSKI_TETRAHEDRON_H
#define TRACERGEN_SIERPINSKI_TETRAHEDRON_H

#include "hittable_list.h"
#include "triangle.h"

class SierpinskiTetrahedron {
public:
    SierpinskiTetrahedron() = default;
    static hittable_list create(int depth, const point3& center, double side_length, const shared_ptr<material>& mat);
private:
    static void create_recursive(hittable_list& list, int depth, const point3& center, double side_length, const shared_ptr<material>& mat);
};

void SierpinskiTetrahedron::create_recursive(hittable_list& list, int depth, const point3& center, double side_length, const shared_ptr<material>& mat) {
    if (depth == 0) {
        // Create the base tetrahedron
        point3 A = center + vec3(-side_length/2, 0, -side_length/(2 * sqrt(2)));
        point3 B = center + vec3(side_length/2, 0, -side_length/(2 * sqrt(2)));
        point3 C = center + vec3(0, 0, side_length/sqrt(2));
        point3 D = center + vec3(0, side_length * sqrt(2.0/3.0), 0);

        list.add(make_shared<triangle>(A, B, C, mat));
        list.add(make_shared<triangle>(A, B, D, mat));
        list.add(make_shared<triangle>(A, C, D, mat));
        list.add(make_shared<triangle>(B, C, D, mat));
    } else {
        // Recursive case
        double new_side_length = side_length / 2;

        create_recursive(list, depth - 1, center + vec3(-new_side_length / 4, -new_side_length * sqrt(2.0/12.0), -new_side_length / (4 * sqrt(2))), new_side_length, mat);
        create_recursive(list, depth - 1, center + vec3(new_side_length / 4, -new_side_length * sqrt(2.0/12.0), -new_side_length / (4 * sqrt(2))), new_side_length, mat);
        create_recursive(list, depth - 1, center + vec3(0, -new_side_length * sqrt(2.0/12.0), new_side_length / (2 * sqrt(2))), new_side_length, mat);
        create_recursive(list, depth - 1, center + vec3(0, new_side_length * sqrt(2.0/3.0) / 2, 0), new_side_length, mat);
    }
}

hittable_list SierpinskiTetrahedron::create(int depth, const point3& center, double side_length, const shared_ptr<material>& mat) {
    hittable_list tetrahedron_list;
    create_recursive(tetrahedron_list, depth, center, side_length, mat);
    return tetrahedron_list;
}

#endif //TRACERGEN_SIERPINSKI_TETRAHEDRON_H
