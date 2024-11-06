#ifndef NBODY_H
#define NBODY_H
#include "point.hpp"
#include "qtree.hpp" // Incluir QuadTree y Quad para establecer límites

struct NBody{
public:   
    NBody(int n, double G, double dt, double softening_factor, Quad boundary);
    void update_position(Point *points);
    void update_force(Point *points);
    void update_velocity(Point *points);
    void reset_forces(Point *points);
    void simulate_fb(Point *points);
    ~NBody();

private:
    int n;
    double G;
    double dt;
    double softening_factor;
    Quad boundary_; // Agregar el Quad como límite
};

#endif