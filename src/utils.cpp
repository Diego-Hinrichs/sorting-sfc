#include "utils.hpp"
#include "point.hpp"
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

void initialize_points_gaussian(Point* points, int n, unsigned int seed, double mean = 0.0, double std_dev = 0.1) {
    srand(seed);
    for (int i = 0; i < n; ++i) {
        double u1 = static_cast<double>(rand()) / RAND_MAX;
        double u2 = static_cast<double>(rand()) / RAND_MAX;
        double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
        double z1 = sqrt(-2.0 * log(u1)) * sin(2.0 * M_PI * u2);
        double x = mean + z0 * std_dev;
        double y = mean + z1 * std_dev;
        points[i] = Point(x, y, 0.0, 1.0);
    }
}

void initialize_points_circle(Point* points, int n, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < n; ++i) {
        double angle = static_cast<double>(rand()) / RAND_MAX * 2.0 * M_PI;
        double radius = sqrt(static_cast<double>(rand()) / RAND_MAX); // Distribución uniforme en el área
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        points[i] = Point(x, y, 0.0, 1.0);
    }
}

void initialize_points_ring(Point* points, int n, unsigned int seed, double inner_radius = 0.5, double outer_radius = 1.0) {
    srand(seed);
    for (int i = 0; i < n; ++i) {
        double angle = static_cast<double>(rand()) / RAND_MAX * 2.0 * M_PI;
        double radius = inner_radius + (outer_radius - inner_radius) * (static_cast<double>(rand()) / RAND_MAX);
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        points[i] = Point(x, y, 0.0, 1.0);
    }
}

void initialize_points_clusters(Point* points, int n, unsigned int seed, int clusters = 3, double cluster_radius = 0.1) {
    srand(seed);
    Point* cluster_centers = new Point[clusters];
    for (int i = 0; i < clusters; ++i) {
        double cx = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        double cy = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        cluster_centers[i] = Point(cx, cy, 0.0, 1.0);
    }

    for (int i = 0; i < n; ++i) {
        int cluster_index = rand() % clusters;
        double angle = static_cast<double>(rand()) / RAND_MAX * 2.0 * M_PI;
        double radius = static_cast<double>(rand()) / RAND_MAX * cluster_radius;
        double x = cluster_centers[cluster_index].x + radius * cos(angle);
        double y = cluster_centers[cluster_index].y + radius * sin(angle);
        points[i] = Point(x, y, 0.0, 1.0);
    }
    delete[] cluster_centers;
}

void initialize_points_spiral(Point* points, int n, unsigned int seed, double rotations = 3.0) {
    srand(seed);
    for (int i = 0; i < n; ++i) {
        double t = static_cast<double>(i) / n * rotations * 2.0 * M_PI;
        double radius = static_cast<double>(i) / n; // Crece con el índice
        double x = radius * cos(t);
        double y = radius * sin(t);
        points[i] = Point(x, y, 0.0, 1.0);
    }
}

void initialize_points_line(Point* points, int n, unsigned int seed, double slope = 1.0, double intercept = 0.0) {
    srand(seed);
    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        double y = slope * x + intercept;
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

void calculate_error(Point *fb, Point *bh, int n, int k) {
    double total_error = 0.0;
    for (int i = 0; i < n; ++i) {
        double dx = fb[i].x - bh[i].x;
        double dy = fb[i].y - bh[i].y;
        double distance_error = sqrt(dx * dx + dy * dy);
        total_error += distance_error;
    }
    double mean_error = total_error / n;
    printf("%i;%.15lf\n", k, mean_error);
}

double kinetic_energy(Point* fb, int n) {
    double k_energy = 0.0;
    for(int i = 0; i < n; ++i){ 
        k_energy += 0.5 * fb[i].mass * (fb[i].vx * fb[i].vx + fb[i].vy * fb[i].vy);
    }
    return k_energy;
}

double potential_energy(Point* fb, int n, double G){
    double p_energy = 0.0;
    for(int i = 0; i < n; ++i){
        for (int j = i + 1; j < n; ++j){
            double dx = fb[i].x - fb[j].x;
            double dy = fb[i].y - fb[j].y;
            double dz = fb[i].z - fb[j].z;
            double dist = sqrt(dx * dx + dy * dy + dz * dz);
            p_energy += -G * fb[i].mass * fb[j].mass / dist;
        }
    }
    return p_energy;
}

double total_energy(Point *fb, int n){
    return kinetic_energy(fb, n) + potential_energy(fb, n, 1.0);
}
