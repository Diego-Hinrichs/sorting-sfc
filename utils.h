#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>
#include <algorithm>
#include <math.h>

const float G = 1.0f;

struct Body {
    float x, y; // Position
    float vx, vy; // Velocity
    float fx, fy;   // fuerza acumulada
    float mass;
    
    Body() : x(0), y(0), vx(0), vy(0), fx(0), fy(0), mass(0) {}

    Body(float x, float y, float vx, float vy, float fx, float fy, float mass)
        : x(x), y(y), vx(vx), vy(vy), fx(fx), fy(fy), mass(mass) {}

    
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
unsigned int calculateZOrderIndex(const Body &body);
bool compareZOrder(Body a, Body b);
void sortBodiesZOrder(Body *bodies, int n);

// HILBERT
uint32_t interleave(uint32_t x);
uint32_t hilbertXYZToIndex(uint32_t x, uint32_t y);
unsigned int calculateHilbertIndex(const Body &body);
void sortBodiesHilbert(Body *bodies, int n);

#endif