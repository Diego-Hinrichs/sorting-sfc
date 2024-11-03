#ifndef NBODY_H
#define NBODY_H
#include "point.hpp"

struct NBody{
public:   
    NBody(int n, double G, double dt, double softening_factor);
    void updateForce(Point *points);
    void updatePosition(Point *points);
    void simulateFB(Point *points);
    ~NBody();

private:
    int n;
    double G;
    double dt;
    double softening_factor;
};

#endif

