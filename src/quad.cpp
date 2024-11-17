#include "quad.hpp"
#include <iostream>
Quad::Quad(double x, double y, double w, double h)
    : x(x), y(y), w(w), h(h) {}

bool Quad::contains(const Point& p) { 
    return (p.x >= x - w && p.x <= x + w && p.y >= y - h && p.y <= y + h);
}

Quad Quad::get_sub_quad(int quadrant) {
    switch (quadrant) {
        case 0: // NW
            return Quad(x - w / 2, y + h / 2, w / 2, h / 2);
        case 1: // NE
            return Quad(x + w / 2, y + h / 2, w / 2, h / 2);
        case 2: // SW
            return Quad(x - w / 2, y - h / 2, w / 2, h / 2);
        case 3: // SE
            return Quad(x + w / 2, y - h / 2, w / 2, h / 2);
        default:
            std::cerr << "Error: Invalid quadrant " << quadrant << std::endl;
            return *this;
    }
}
