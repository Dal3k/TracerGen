#ifndef TRACERGEN_MATERIAL_H
#define TRACERGEN_MATERIAL_H

#include "utility.h"
#include "hittable.h"

class material {
public:
    virtual bool scatter(
            const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered
    ) const = 0;
};

class lambertian : public material {
public:
    lambertian(const color &a) : albedo(a) {}

    virtual bool scatter(
            const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    color albedo;
};

#endif //TRACERGEN_MATERIAL_H