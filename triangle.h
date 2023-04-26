#ifndef TRACERGEN_TRIANGLE_H
#define TRACERGEN_TRIANGLE_H

#include "hittable.h"
#include "vec3.h"

class triangle : public hittable {
public:
    triangle() {}

    triangle(const point3& _v0, const point3& _v1, const point3& _v2, shared_ptr<material> mat)
            : v0(_v0), v1(_v1), v2(_v2), mat_ptr(mat) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;
        vec3 h = cross(r.direction(), edge2);
        double a = dot(edge1, h);

        if (a > -epsilon && a < epsilon)
            return false; // This ray is parallel to this triangle.

        double f = 1.0 / a;
        vec3 s = r.origin() - v0;
        double u = f * dot(s, h);
        if (u < 0.0 || u > 1.0)
            return false;

        vec3 q = cross(s, edge1);
        double v = f * dot(r.direction(), q);
        if (v < 0.0 || u + v > 1.0)
            return false;

        double t = f * dot(edge2, q);
        if (t < t_min || t > t_max)
            return false;

        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal = cross(edge1, edge2);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        point3 min_point(fmin(v0.x(), fmin(v1.x(), v2.x())),
                         fmin(v0.y(), fmin(v1.y(), v2.y())),
                         fmin(v0.z(), fmin(v1.z(), v2.z())));

        point3 max_point(fmax(v0.x(), fmax(v1.x(), v2.x())),
                         fmax(v0.y(), fmax(v1.y(), v2.y())),
                         fmax(v0.z(), fmax(v1.z(), v2.z())));

        output_box = aabb(min_point, max_point);
        return true;
    }

public:
    point3 v0, v1, v2;
    shared_ptr<material> mat_ptr;
    const double epsilon = 1e-8;
};

#endif // TRACERGEN_TRIANGLE_H
