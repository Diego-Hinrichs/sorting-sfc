#ifndef NBODY_H
#define NBODY_H
#include "point.hpp"

struct NBody{
public:   
    NBody(int n, float G, float dt, float softening_factor);
    void updateForce(Point *points);
    void updatePosition(Point *points);
    void simulateFB(Point *points);
    ~NBody();

private:
    int n;
    float G;
    float dt;
    float softening_factor;
};

#endif

