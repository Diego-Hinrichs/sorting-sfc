#include "utils.hpp"
#include <iostream>

#define EPSILON 1e-4 // Tolerance for the error

void initialize_points(Point* points, int n, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        double y = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        points[i] = Point(x, y, 0.0, 1.0);
    }
}

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

void print_points(Point* fb, Point* bh, int n) {
    for (int i = 0; i < n; ++i) {
        printf("fb[%i] = (%10.7lf, %10.7lf)\t", i, fb[i].x, fb[i].y);
        printf("bh[%i] = (%10.7lf, %10.7lf)\n", i, bh[i].x, bh[i].y);
    }
}

void print_arrays(Point* fb, Point* bh, int n, int k) {
    int m = n < 100 ? n : 100;
    std::cout << "fb_data = [";
    for (int i = 0; i < m; ++i) {
        if (i % 3 == 0) {
            std::cout << "\n\t";
        }
        printf("(%lf, %lf), ", fb[i].x, fb[i].y);
    }
    std::cout << "\n]" << std::endl;
    std::cout << "\nbh_data = [";
    for (int i = 0; i < m; ++i) {
        if (i % 3 == 0) {
            std::cout << "\n\t";
        }
        printf("(%lf, %lf), ", bh[i].x, bh[i].y);
    }
    std::cout << "\n]\n" << std::endl;
    std::cout << "n = " << n << std::endl;
    std::cout << "step = " << k << std::endl;
}

void calculate_error(Point *fb, Point *bh, int n) {
    double total_error = 0.0;
    for (int i = 0; i < n; ++i) {
        double dx = fb[i].x - bh[i].x;
        double dy = fb[i].y - bh[i].y;
        double distance_error = sqrt(dx * dx + dy * dy);
        total_error += distance_error;
    }
    double mean_error = total_error / n;

    if (mean_error > EPSILON) {
        std::cout << "Error promedio (" << mean_error << "). Excede la tolerancia: (" << EPSILON << ")" << std::endl;
    } else if (mean_error < EPSILON) {
        std::cout << "Error promedio (" << mean_error << "). Tolerancia: (" << EPSILON << ")" << std::endl;
    }
}

