#ifndef UTILS_H
#define UTILS_H
#include "point.hpp"
#include <iostream>

void update_position(Point* points, int n, double dt);
void reset_forces(Point* points, int n);

void initialize_points(Point* points, int n, unsigned int seed);
bool debug_mode(int n, int k, int x, int max_capacity);
void print_arrays(Point* fb, Point* bh, int n, int k);
void print_points(Point* fb, Point* bh, int n);
void calculate_error(Point *fb, Point *bh, int n);

#endif
