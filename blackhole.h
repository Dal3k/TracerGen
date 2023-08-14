#ifndef TRACERGEN_BLACKHOLE_H
#define TRACERGEN_BLACKHOLE_H


class Blackhole : public hittable {
public:
    Blackhole() {}

    Blackhole(point3 cen, double r) : center(cen), radius(r) {}

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const override {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        auto root = (-half_b - sqrtd) / a;
        if (root < tmin || tmax < root) {
            root = (-half_b + sqrtd) / a;
            if (root < tmin || tmax < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);

        return true;
    }

    bool bounding_box(double time0, double time1, aabb &output_box) const {
        output_box = aabb(
                center - vec3(radius, radius, radius),
                center + vec3(radius, radius, radius));
        return true;
    }


    virtual ray interact(const ray& r, const hit_record& rec) const override {
        vec3 to_center = center - rec.p;
        vec3 new_direction = r.direction() + 2 * to_center * dot(to_center, r.direction()) / dot(to_center, to_center);
        return ray(rec.p, new_direction, r.time());
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
        vec3 r_to_center = center - r.origin();
        double distance = r_to_center.length();

        // Assume deflection angle is proportional to radius / distance^2
        return radius / (distance * distance);
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

private:
    point3 center;
    double radius; // We'll use radius to compute deflection angle.
};

#endif //TRACERGEN_BLACKHOLE_H
