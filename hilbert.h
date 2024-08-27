#ifndef HILBERT_H
#define HILBERT_H

#include <iostream>
#include <algorithm>
#include <cstdint>

struct Body {
    float x, y; // Coordenadas en 2D
    float vx, vy;
    float mass;
};

typedef uint32_t peanokey;

peanokey peano_hilbert_key(int x, int y, int bits);
bool compareHilbertOrder(const Body& a, const Body& b);
void sortBodiesHilbert(Body* bodies, unsigned long n);

#endif