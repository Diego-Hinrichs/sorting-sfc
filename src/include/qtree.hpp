#ifndef QTREE_H
#define QTREE_H
#include "qtnode.hpp"

// TODO: MAKE A SIMULATE_BH FUNCTION, LIKE FB
class QuadTree {
public:
    QuadTreeNode* root;
    int capacity;
    int n;
    double G;
    double dt;
    double softening_factor;

    QuadTree(){};
    QuadTree(int n, double G, double delta_time, double softening_factor, int capacity, Quad boundary)
        : capacity(capacity), n(n), G(G), dt(delta_time), softening_factor(softening_factor) {
        root = new QuadTreeNode(boundary, capacity);
    }

    ~QuadTree() {
        delete root;
    }

    void insert(Point* points) {
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            root->insert(points, i);
        }
    }

    void update_force(Point* points, double THETA) {
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            root->calculate_force_node(points, i, softening_factor, THETA, G);
        }
    }
    
    // TODO: modificar para 3D.
    void update_velocity(Point* points) {
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i){
            points[i].update_velocity(dt);
        }
    }

    void update_position(Point* points) {
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i){
            points[i].update_position(dt);
        }
    }

    void reset_forces(Point* points) {
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i){
            points[i].reset_forces();
        }
    }

    void clear() {
        root->clear();
    }
};

#endif