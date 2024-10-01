#ifndef BODY_H
#define BODY_H

#include <math.h>
#include <random>
#include <algorithm> // std::sort morton.cpp y hilbert.cpp

typedef uint32_t coord_t;

// no deberia estar aqui
const float G = 1.0f;

struct Body {
    float x, y;     // Position
    float vx, vy;   // Velocity
    float fx, fy;   // fuerza acumulada
    float mass;
    
    Body() : x(0), y(0), vx(0), vy(0), fx(0), fy(0), mass(0) {}

    Body(float x, float y, float vx, float vy, float fx, float fy, float mass)
        : x(x), y(y), vx(vx), vy(vy), fx(fx), fy(fy), mass(mass) {}

    // TODO: Esto es para Barnes-Hut, ver donde dejar para le fuerza bruta
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

#endif