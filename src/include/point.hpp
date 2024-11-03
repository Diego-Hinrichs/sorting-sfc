#ifndef POINT_H
#define POINT_H

struct Point {
public:
    double x, y, z;     // Posición
    double vx, vy, vz;  // Velocidad
    double fx, fy, fz;  // Velocidad
    double mass;        // Masa

    Point(double x, double y, double z, double mass)
        : x(x), y(y), z(z), vx(0.0f), vy(0.0f), vz(0.0f), fx(0.0f), fy(0.0f), fz(0.0f), mass(mass) {}

    Point() : x(0), y(0), z(0), vy(0.0f), vz(0.0f), fx(0.0f), fy(0.0f), fz(0.0f), mass(1.0f) {}

    void reset_points_force(){
        fx = 0.0f;
        fy = 0.0f;
        fz = 0.0f;
    }
};

#endif