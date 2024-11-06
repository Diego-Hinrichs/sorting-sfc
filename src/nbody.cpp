#include <cmath>
#include "nbody.hpp"

NBody::NBody(int n, double G, double dt, double softening_factor, Quad boundary)
    : n(n), G(G), dt(dt), softening_factor(softening_factor), boundary_(boundary) {}

void NBody::update_force(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i)
    {
        if (!boundary_.contains(points[i])) {
            continue;
        }

        for (int j = 0; j < n; ++j)
        {
            if (i == j || !boundary_.contains(points[j])) // Verificar si los puntos están dentro del Quad
                continue;

            double dx = points[j].x - points[i].x;
            double dy = points[j].y - points[i].y;
            double dist_sq = dx * dx + dy * dy + softening_factor * softening_factor;
            double dist = sqrt(dist_sq);
            double inv_dist = 1.0 / dist;
            double F = G * points[j].mass * inv_dist * inv_dist;

            // A = F / m_i
            points[i].fx += F * dx;
            points[i].fy += F * dy;
        }
    }
}

void NBody::update_position(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i){
        if (boundary_.contains(points[i])) {
            points[i].x += points[i].vx * dt;
            points[i].y += points[i].vy * dt;
        }
    }
}
void NBody::update_velocity(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i){
        points[i].vx += points[i].fx * dt;
        points[i].vy += points[i].fy * dt;
    }
}

void  NBody::reset_forces(Point *points){
    // #pragma omp parallel for
    for (int i = 0; i < n; ++i){
        points[i].fx = 0.0;
        points[i].fy = 0.0;
    }
}

void NBody::simulate_fb(Point *points){
    update_force(points);
    update_position(points);
    update_velocity(points);
    reset_forces(points);
}

NBody::~NBody(){}