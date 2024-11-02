#ifndef POINT_H
#define POINT_H

class Point {
public:
    float x, y, z;     // Posición
    float vx, vy, vz;  // Velocidad
    float mass;        // Masa

    Point(float x, float y, float z, float mass)
        : x(x), y(y), z(z), vx(0.0f), vy(0.0f), vz(0.0f), mass(mass) {}

    Point() : x(0), y(0), z(0), vy(0.0f), vz(0.0f), mass(1.0f) {}
};

#endif