#ifndef TRACERGEN_BLACKHOLE_H
#define TRACERGEN_BLACKHOLE_H

#include "hittable.h"

class Blackhole : public hittable {
public:
    Blackhole() {}

    Blackhole(point3 cen, double m) : center(cen), mass(m) {}

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const override {
        // Use a sphere's hit detection logic for simplicity
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - mass*mass;

        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < tmin || tmax < root) {
            root = (-half_b + sqrtd) / a;
            if (root < tmin || tmax < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / mass;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

    ray deflect_ray(const ray& r) const {
        double angle = computeDeflection(r);
        vec3 new_direction = rotate(r.direction(), angle);
        return ray(r.origin(), new_direction, r.time());
    }

    point3 translate_space(const point3& p) const {
        // Translate space so that the black hole is at the origin
        return p - center;
    }

    double computeDeflection(const ray& r) const {
        // Calculate deflection angle based on gravitational lensing.
        // For simplicity, we assume that black hole's mass is directly proportional to deflection angle.
        // Note that this is a simplification and not physically accurate.
        vec3 r_to_center = center - r.origin();
        double distance = r_to_center.length();

        // Assume deflection angle is proportional to mass / distance^2
        return mass / (distance * distance);
    }

    vec3 rotate(const vec3& v, double angle) const {
        // Rotate vector v by a given angle.
        // For simplicity, we'll rotate around the z-axis.
        // Note: this rotation assumes that the black hole is located at the origin.
        double s = sin(angle);
        double c = cos(angle);

        double new_x = v.x() * c - v.y() * s;
        double new_y = v.x() * s + v.y() * c;

        return vec3(new_x, new_y, v.z());
    }

public:
    point3 center;
    double mass; // We'll use mass to compute deflection angle.
};

#endif //TRACERGEN_BLACKHOLE_H
