#ifndef QUAD_H
#define QUAD_H
#include "point.hpp"

struct Quad {
public:
    float x, y;
    float half_width, half_height;

    Quad(float x, float y, float half_width, float half_height);
    bool contains(Point& p);
    Quad get_sub_quad(int quadrant);
};

#endif