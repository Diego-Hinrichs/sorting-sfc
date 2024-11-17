#ifndef QUAD_H
#define QUAD_H
#include "point.hpp"

struct Quad {
public:
    double x, y; // center
    double w, h; // (x + w, y + h) limits 

    Quad(double x, double y, double w, double h);
    bool contains(const Point& p);
    Quad get_sub_quad(int quadrant);
};

#endif