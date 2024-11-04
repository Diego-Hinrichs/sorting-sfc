#include "quad.hpp"

Quad::Quad(double x, double y, double half_width, double half_height)
    : x(x), y(y), half_width(half_width), half_height(half_height) {}

bool Quad::contains(Point& p) {
    return (p.x >= x - half_width && p.x < x + half_width &&
                p.y >= y - half_height && p.y < y + half_height);    
};

Quad Quad::get_sub_quad(int quadrant) {
    double newHalfWidth = half_width / 2.0;
    double newHalfHeight = half_height / 2.0;

    switch (quadrant) {
        case 0: return Quad(x - newHalfWidth, y - newHalfHeight, newHalfWidth, newHalfHeight); // NW
        case 1: return Quad(x + newHalfWidth, y - newHalfHeight, newHalfWidth, newHalfHeight); // NE
        case 2: return Quad(x - newHalfWidth, y + newHalfHeight, newHalfWidth, newHalfHeight); // SW
        case 3: return Quad(x + newHalfWidth, y + newHalfHeight, newHalfWidth, newHalfHeight); // SE
        default: return *this; // Fallback
    }
}
