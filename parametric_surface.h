#ifndef PARAMETRIC_SURFACE_H
#define PARAMETRIC_SURFACE_H

#include "hittable.h"
#include "vec3.h"

class parametric_surface : public hittable {
public:
    parametric_surface() {}
    parametric_surface(
            std::function<point3(double, double)> parametric_function,
            double u_min, double u_max,
            double v_min, double v_max,
            std::shared_ptr<material> mat
    ) : func(parametric_function), u0(u_min), u1(u_max), v0(v_min), v1(v_max), mat_ptr(mat) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

private:
    std::function<point3(double, double)> func;
    double u0, u1, v0, v1;
    std::shared_ptr<material> mat_ptr;
};

bool parametric_surface::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    // Implement a ray-parametric surface intersection algorithm.
    // Depending on the specific parametric functions used, you might
    // need to use iterative numerical methods like Newton-Raphson.
    // This is a complex task and beyond the scope of this response.
    return false;
}

bool parametric_surface::bounding_box(double t0, double t1, aabb &output_box) const {
    // Compute the bounding box of the parametric surface.
    // This depends on the specific parametric function you choose.
    // For example, you can analytically compute the bounding box
    // or use an approximate bounding box based on the domain.
    return false;
}

#endif // PARAMETRIC_SURFACE_H
