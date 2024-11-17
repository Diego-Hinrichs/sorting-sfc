#ifndef NBODY_H
#define NBODY_H
#include "utils.hpp"

struct NBody{
public:   
    NBody(int n, double G, double dt, double softening_factor);
    void update_force(Point *points);
    void simulate_fb(Point *points);
    ~NBody();

private:
    int n;
    double G;
    double dt;
    double softening_factor;
};

#endif