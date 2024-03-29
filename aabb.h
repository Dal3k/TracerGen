#ifndef TRACERGEN_AABB_H
#define TRACERGEN_AABB_H

#include "utility.h"

class aabb {
public:
    aabb() {}

    aabb(const point3 &a, const point3 &b) {
        minimum = a;
        maximum = b;
    }

    point3 min() const { return minimum; }

    point3 max() const { return maximum; }

    bool hit(const ray &r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / r.direction()[a];
            auto t0 = (min()[a] - r.origin()[a]) * invD;
            auto t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    point3 minimum;
    point3 maximum;

    int longest_axis() const {
        vec3 extents = max() - min();
        if (extents.x() >= extents.y() && extents.x() >= extents.z()) {
            return 0;
        } else if (extents.y() >= extents.x() && extents.y() >= extents.z()) {
            return 1;
        } else {
            return 2;
        }
    }

    double area() const {
        auto a = max().x() - min().x();
        auto b = max().y() - min().y();
        auto c = max().z() - min().z();
        return 2 * (a * b + b * c + c * a);
    }

};

inline aabb surrounding_box(aabb box0, aabb box1) {
    point3 small(fmin(box0.min().x(), box1.min().x()),
                 fmin(box0.min().y(), box1.min().y()),
                 fmin(box0.min().z(), box1.min().z()));

    point3 big(fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));

    return aabb(small, big);
}

#endif //TRACERGEN_AABB_H
