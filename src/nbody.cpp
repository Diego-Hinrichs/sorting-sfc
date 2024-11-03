#include <cmath>
#include "nbody.hpp"

NBody::NBody(int n, double G, double dt, double softening_factor)
    : n(n), G(G), dt(dt), softening_factor(softening_factor) {}

void NBody::updateForce(Point *points){
#pragma omp parallel for
    for (int i = 0; i < n; ++i)
    {
        double fx = 0.0f;
        double fy = 0.0f;

        for (int j = 0; j < n; ++j)
        {
            if (i == j)
                continue;

            double dx = points[j].x - points[i].x;
            double dy = points[j].y - points[i].y;

            double dist_square = dx * dx + dy * dy + softening_factor;
            double dist_square_cube = dist_square * dist_square * dist_square;
            double dist = sqrt(dist_square_cube);

            double F = G * points[j].mass / dist;

            // A = F / m_i
            fx += F * dx / dist;
            fy += F * dy / dist;
        }
        // V = A * dt
        points[i].vx += fx * dt;
        points[i].vy += fy * dt;
    }
}

void NBody::updatePosition(Point *points){
#pragma omp parallel for
    for (int i = 0; i < n; ++i){
        points[i].x += points[i].vx * dt;
        points[i].y += points[i].vy * dt;
    }
}

void NBody::simulateFB(Point *points){
    updateForce(points);
    updatePosition(points);
}

NBody::~NBody(){}
