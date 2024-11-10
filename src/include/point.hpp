#ifndef POINT_H
#define POINT_H
#include <math.h>

struct Point {
public:
    double x, y, z;     // Posición
    double vx, vy, vz;  // Velocidad
    double fx, fy, fz;  // Fuerza
    double mass;        // Masa

    Point(double x, double y, double z, double mass)
        : x(x), y(y), z(z), vx(0.0), vy(0.0), vz(0.0), fx(0.0), fy(0.0), fz(0.0), mass(mass) {}

    // particle-particle force
    void add_force(Point& p, double softening_factor, double G) {
        double dx = p.x - this->x;
        double dy = p.y - this->y;
        double dist_sq = dx * dx + dy * dy + softening_factor * softening_factor;
        double dist = sqrt(dist_sq);
        double inv_dist = 1.0 / dist;
        double F = G * p.mass * inv_dist * inv_dist;

        // A = F / m_i
        this->fx += F * dx;
        this->fy += F * dy;
    }

    void update_velocity(double dt) {
        vx += fx * dt;
        vy += fy * dt;
        vz += fz * dt;
    }

    void update_position(double dt) {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
    }
    
    void reset_forces() {
        fx = 0.0;
        fy = 0.0;
        fz = 0.0;
    }

};

#endif