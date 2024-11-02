#include <cmath>
#include "nbody.hpp"

NBody::NBody(int n, float G, float dt, float softening_factor)
    : n(n), G(G), dt(dt), softening_factor(softening_factor) {}

void NBody::updateForce(Point *points){
#pragma omp parallel for
    for (int i = 0; i < n; ++i)
    {
        float fx = 0.0f;
        float fy = 0.0f;

        for (int j = 0; j < n; ++j)
        {
            if (i == j)
                continue;

            float dx = points[j].x - points[i].x;
            float dy = points[j].y - points[i].y;

            float distSq = dx * dx + dy * dy;
            distSq += softening_factor * softening_factor;
            float distSq3 = distSq * distSq;

            float dist = sqrt(distSq3);
            float invDist = 1.0f / dist;

            float F = G * points[j].mass * invDist;

            // A = F / m_i
            fx += F * dx;
            fy += F * dy;
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
