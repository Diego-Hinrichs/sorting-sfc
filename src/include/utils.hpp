#ifndef UTILS_H
#define UTILS_H
#include "point.hpp"
#include <iostream>

void update_position(Point* points, int n, double dt);
void reset_forces(Point* points, int n);

void initialize_points(Point* points, int n, unsigned int seed);
void initialize_points_gaussian(Point* points, int n, unsigned int seed, double mean, double std_dev);
void initialize_points_circle(Point* points, int n, unsigned int seed);
void initialize_points_ring(Point* points, int n, unsigned int seed, double inner_radius, double outer_radius);
void initialize_points_clusters(Point* points, int n, unsigned int seed, int clusters, double cluster_radius);
void initialize_points_spiral(Point* points, int n, unsigned int seed, double rotations);
void initialize_points_line(Point* points, int n, unsigned int seed, double slope, double intercept); 
bool debug_mode(int n, int k, int x, int max_capacity);
void print_points(Point* fb, Point* bh, int n);
double kinetic_energy(Point* fb, int n);
double potential_energy(Point* fb, int n, double G);
double total_energy(Point *fb, int n);
void calculate_error(Point *fb, Point *bh, int n, int k);

#endif
