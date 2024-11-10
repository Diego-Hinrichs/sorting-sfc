#include <cmath>
#include "nbody.hpp"

NBody::NBody(int n, double G, double dt, double softening_factor, Quad boundary)
    : n(n), G(G), dt(dt), softening_factor(softening_factor), boundary_(boundary) {}

void NBody::update_force(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        // if (!boundary_.contains(points[i])) {
        //     continue;
        // }
        for (int j = 0; j < n; ++j) {
            if (i == j) // verificar si los puntos son iguales
            // if (i == j || !boundary_.contains(points[j])) // verificar si los puntos están dentro del Quad
                continue;

            points[i].add_force(points[j], softening_factor, G);
        }
    }
}

void NBody::update_velocity(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i){
        points[i].update_velocity(dt);
    }
}

void NBody::update_position(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i){
        points[i].update_position(dt);
    }
}

void  NBody::reset_forces(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i){
        points[i].reset_forces();
    }
}

void NBody::simulate_fb(Point *points){
    update_force(points);
    update_velocity(points);
    update_position(points);
    reset_forces(points);
}

NBody::~NBody(){}