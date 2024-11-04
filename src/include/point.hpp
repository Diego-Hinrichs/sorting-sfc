#ifndef POINT_H
#define POINT_H

struct Point {
public:
    double x, y, z;     // Posición
    double vx, vy, vz;  // Velocidad
    double fx, fy, fz;  // Fuerza
    double mass;        // Masa

    Point(double x, double y, double z, double mass)
        : x(x), y(y), z(z), vx(0.0), vy(0.0), vz(0.0), fx(0.0), fy(0.0), fz(0.0), mass(mass) {}

};

#endif