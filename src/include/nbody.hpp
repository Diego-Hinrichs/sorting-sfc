#ifndef NBODY_H
#define NBODY_H
#include "point.hpp"
#include "qtree.hpp" // Incluir QuadTree y Quad para establecer límites

struct NBody{
public:   
    NBody(int n, double G, double dt, double softening_factor, Quad boundary);
    void updateForce(Point *points);
    void updatePosition(Point *points);
    void simulateFB(Point *points);
    ~NBody();

private:
    int n;
    double G;
    double dt;
    double softening_factor;
    Quad boundary_; // Agregar el Quad como límite
};

#endif