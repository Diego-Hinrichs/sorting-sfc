#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <random>

const float G = 1.0f;

typedef uint32_t coord_t;

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
void AxestoTranspose2D(coord_t* X, int b);
bool compareHilbertOrder(const Body& a, const Body& b, int bits);
void sortBodiesHilbert(Body* bodies, unsigned long n, int bits);

void initBodies(Body *bodies, int numBodies, unsigned int seed);
#endif