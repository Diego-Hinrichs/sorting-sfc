#include "quad.hpp"
Quad::Quad(double x, double y, double w, double h)
    : x(x), y(y), w(w), h(h) {}

bool Quad::contains(const Point& p) { 
    return (p.x >= x - w && p.x <= x + w && p.y >= y - h && p.y <= y + h);
}

Quad Quad::get_sub_quad(int quadrant) {
    switch (quadrant) {
        case 0: // NW (North-West)
            return Quad(x - w / 2, y + h / 2, w / 2, h / 2);
        case 1: // NE (North-East)
            return Quad(x + w / 2, y + h / 2, w / 2, h / 2);
        case 2: // SW (South-West)
            return Quad(x - w / 2, y - h / 2, w / 2, h / 2);
        case 3: // SE (South-East)
            return Quad(x + w / 2, y - h / 2, w / 2, h / 2);
        default:
            return *this;
    }
}
