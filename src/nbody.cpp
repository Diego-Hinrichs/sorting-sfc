#include <cmath>
#include "nbody.hpp"

NBody::NBody(int n, double G, double dt, double softening_factor)
    : n(n), G(G), dt(dt), softening_factor(softening_factor) {}

void NBody::update_force(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j)
                continue;
            points[i].add_force(points[j], softening_factor, G);
        }
    }
}

void NBody::simulate_fb(Point *points){
    reset_forces(points, n);
    update_force(points);
    update_position(points, n, dt);  
}

NBody::~NBody(){}