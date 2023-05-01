// barnsley_fern.h

#ifndef TRACERGEN_BARNSLEY_FERN_H
#define TRACERGEN_BARNSLEY_FERN_H

#include "hittable.h"
#include "hittable_list.h"
#include "cylinder.h"
#include "sphere.h"
#include <random>

class BarnsleyFern : public hittable {
public:
    BarnsleyFern(int num_points, double scale, shared_ptr<material> mat);

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

private:
    hittable_list fern_parts;
    std::pair<double, double> iterate_point(double x, double y) const;
};

BarnsleyFern::BarnsleyFern(int num_points, double scale, shared_ptr<material> mat) {
    double x = 0;
    double y = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < num_points; ++i) {
        auto [x_new, y_new] = iterate_point(x, y);
        x = x_new;
        y = y_new;

        point3 p(x * scale, y * scale, 0);
        fern_parts.add(make_shared<sphere>(p, scale * 0.01, mat));
    }
}

bool BarnsleyFern::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return fern_parts.hit(r, t_min, t_max, rec);
}

bool BarnsleyFern::bounding_box(double t0, double t1, aabb& output_box) const {
    return fern_parts.bounding_box(t0, t1, output_box);
}

std::pair<double, double> BarnsleyFern::iterate_point(double x, double y) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double r = dis(gen);

    if (r < 0.01) {
        return {0, 0.16 * y};
    } else if (r < 0.86) {
        return {0.85 * x + 0.04 * y, -0.04 * x + 0.85 * y + 1.6};
    } else if (r < 0.93) {
        return {0.2 * x - 0.26 * y, 0.23 * x + 0.22 * y + 1.6};
    } else {
        return {-0.15 * x + 0.28 * y, 0.26 * x + 0.24 * y + 0.44};
    }
}

#endif // TRACERGEN_BARNSLEY_FERN_H
