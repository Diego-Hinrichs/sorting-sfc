#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>
#include <vector>
#include <algorithm>
#include <cmath>

const float G = 1.0f; // Constante de gravitación universal

struct Body {
    float x, y;     // posición
    float vx, vy;   // velocidad
    float mass;     // masa
    float fx, fy;   // fuerza acumulada

    Body() : x(0), y(0), vx(0), vy(0), mass(0), fx(0), fy(0) {}
    
    Body(float x, float y, float vx, float vy, float mass)
        : x(x), y(y), vx(vx), vy(vy), mass(mass), fx(0), fy(0) {}

    void resetForce() {
        fx = 0;
        fy = 0;
    }

    void addForce(const Body &b) {
        float dx = b.x - x;
        float dy = b.y - y;
        float dist = std::sqrt(dx*dx + dy*dy + 0.09);
        float F = (G * b.mass) / dist;
        fx += F * dx / dist;
        fy += F * dy / dist;
    }

    void update(float dt) {
        vx += fx * dt;
        vy += fy * dt;
        x += vx * dt;
        y += vy * dt;
    }
};

// ZORDER
unsigned int calculateZOrderIndex(const Body &a);
bool compareZOrder(Body a, Body b);
void sortBodiesZOrder(std::vector<Body> &bodies);

// HILBERT
uint32_t interleave(uint32_t x);
uint32_t calculateHilbertIndex(const Body &a);
bool compareHilbertOrder(const Body &a, const Body &b);
void sortBodiesHilbert(std::vector<Body> &bodies);

#endif // INDEX_H