// torus.h
#ifndef TORUS_H
#define TORUS_H

#include "parametric_surface.h"

class torus : public parametric_surface {
public:
    torus(double R, double r, std::shared_ptr<material> mat)
            : parametric_surface(
            [R, r](double u, double v) {
                return point3(
                        (R + r * cos(v)) * cos(u),
                        (R + r * cos(v)) * sin(u),
                        r * sin(v)
                );
            },
            0, 2 * pi, 0, 2 * pi, mat) {}
};

#endif // TORUS_H
