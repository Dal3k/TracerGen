#pragma once

#ifndef TRACERGEN_CYLINDER_H
#define TRACERGEN_CYLINDER_H

#include "hittable.h"

class cylinder : public hittable {
public:
    cylinder() {}
    cylinder(point3 p0, point3 p1, double r, shared_ptr<material> m)
            : base(p0), cap(p1), radius(r), mat_ptr(m) {};

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    point3 base;
    point3 cap;
    double radius;
    shared_ptr<material> mat_ptr;
};

inline void get_cylinder_uv(const vec3& p, double& u, double& v) {
    auto phi = atan2(p.z(), p.x());
    u = 1 - (phi + pi) / (2 * pi);
    v = p.y();
}

bool cylinder::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - base;
    vec3 direction = r.direction();

    auto a = direction.x() * direction.x() + direction.z() * direction.z();
    auto b = 2 * (oc.x() * direction.x() + oc.z() * direction.z());
    auto c = oc.x() * oc.x() + oc.z() * oc.z() - radius * radius;

    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;

    auto sqrtd = sqrt(discriminant);
    auto root = (-b - sqrtd) / (2 * a);

    if (root < t_min || t_max < root) {
        root = (-b + sqrtd) / (2 * a);
        if (root < t_min || t_max < root)
            return false;
    }

    vec3 hit_point = r.at(root);
    double hit_y = hit_point.y();
    if (hit_y < base.y() || hit_y > cap.y()) return false;

    rec.t = root;
    rec.p = hit_point;
    vec3 outward_normal = (rec.p - base) / radius;
    rec.set_face_normal(r, outward_normal);
    get_cylinder_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool cylinder::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(point3(base.x() - radius, base.y(), base.z() - radius),
                      point3(base.x() + radius, base.y() + cap.y() - base.y(), base.z() + radius));
    return true;
}

#endif // TRACERGEN_CYLINDER_H
