#ifndef TRACERGEN_MATERIAL_H
#define TRACERGEN_MATERIAL_H

#include "utility.h"
#include "ray.h"

struct hit_record;

class material {
public:
    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};

#endif //TRACERGEN_MATERIAL_H