#include "utils.hpp"

void update_position(Point* points, int n, double dt) {
    for (int i = 0; i < n; ++i) {
        points[i].update_position(dt);
    }
}

void reset_forces(Point* points, int n) {
    for (int i = 0; i < n; ++i) {
        points[i].reset_forces();
    }
}
