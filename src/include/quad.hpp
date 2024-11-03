#ifndef QUAD_H
#define QUAD_H
#include "point.hpp"

struct Quad {
public:
    double x, y;
    double half_width, half_height;

    Quad(double x, double y, double half_width, double half_height);
    bool contains(Point& p);
    Quad get_sub_quad(int quadrant);
};

#endif