//
// Created by Gaëtan Pusel on 30/04/2023.
//

#ifndef TRACERGEN_FRACTAL_TREE_3D_H
#define TRACERGEN_FRACTAL_TREE_3D_H

#include "hittable.h"
#include "hittable_list.h"
#include "cylinder.h"
#include <string>
#include <vector>
#include <memory>
#include <cmath>


class FractalTree3D : public hittable {
public:
    FractalTree3D(const point3& root, double initial_length, double initial_radius, int iterations, shared_ptr<material> mat);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

private:
    hittable_list tree_parts;
    hittable_list create_tree(const std::string& commands, double length, double radius, const point3& root, const vec3& direction, shared_ptr<material> mat);
    static std::string generate_l_system(int iterations, const std::string& axiom, const std::string& rules);

    vec3 rotate(const vec3 &v, double angle, const vec3 &axis);
};

vec3 FractalTree3D::rotate(const vec3& v, double angle, const vec3& axis) {
    vec3 u = unit_vector(axis);
    double cos_theta = cos(angle);
    double sin_theta = sin(angle);

    return v * cos_theta + cross(u, v) * sin_theta + u * dot(u, v) * (1 - cos_theta);
}


FractalTree3D::FractalTree3D(const point3& root, double initial_length, double initial_radius, int iterations, shared_ptr<material> mat) {
    std::string l_system = generate_l_system(iterations, "X", "F[+X][-X]FX");
    tree_parts = create_tree(l_system, initial_length, initial_radius, root, vec3(0, 1, 0), mat);
}

bool FractalTree3D::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return tree_parts.hit(r, t_min, t_max, rec);
}

bool FractalTree3D::bounding_box(double t0, double t1, aabb& output_box) const {
    return tree_parts.bounding_box(t0, t1, output_box);
}

hittable_list FractalTree3D::create_tree(const std::string& commands, double length, double radius, const point3& root, const vec3& direction, shared_ptr<material> mat) {
    hittable_list tree;
    std::vector<point3> positions;
    std::vector<vec3> directions;
    positions.push_back(root);
    directions.push_back(direction);

    for (char command : commands) {
        switch (command) {
            case 'F':
            {
                point3 new_root = positions.back() + length * directions.back();
                tree.add(make_shared<cylinder>(positions.back(), new_root, radius, mat));
                positions.back() = new_root;
            }
                break;
            case '[':
                positions.push_back(positions.back());
                directions.push_back(directions.back());
                break;
            case ']':
                positions.pop_back();
                directions.pop_back();
                break;
            case '+':
                directions.back() = rotate(directions.back(), M_PI / 6, vec3(1, 0, 0));
                break;
            case '-':
                directions.back() = rotate(directions.back(), -M_PI / 6, vec3(1, 0, 0));
                break;
            case 'X':
                break;
            default:
                break;
        }
    }

    return tree;
}

std::string FractalTree3D::generate_l_system(int iterations, const std::string& axiom, const std::string& rules) {
    std::string l_system = axiom;

    for (int i = 0; i < iterations; ++i) {
        std::string new_system;

        for (char command : l_system) {
            if (command == 'X') {
                new_system += "F[+X][-X]";
            } else {
                new_system += command;
            }
        }

        l_system = new_system;
    }

    return l_system;
}

#endif //TRACERGEN_FRACTAL_TREE_3D_H
