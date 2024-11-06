#ifndef QTREE_H
#define QTREE_H
#include "qtnode.hpp"

// TODO: MAKE A SIMULATE_BH FUNCTION, LIKE FB
class QuadTree {
public:
    QuadTreeNode* root;
    int capacity;

    QuadTree(){};
    QuadTree(Quad boundary, int capacity)
        : capacity(capacity) {
        root = new QuadTreeNode(boundary, capacity);
    }

    ~QuadTree() {
        delete root;
    }

    void insert(Point* points, int n) {
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            root->insert(points, i);
        }
    }

    void calculate_force(Point* points, int n, double softening_factor, double THETA, double G) {
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            root->calculate_force_node(points, i, softening_factor, THETA, G);
        }
    }
    
    // TODO: modificar para 3D.
    void update_velocity(Point* points, int n, double deltaTime) {
        // !! Mover a POINT
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i){
            points[i].vx += points[i].fx * deltaTime;
            points[i].vy += points[i].fy * deltaTime;
        }
    }

    void update_position(Point* points, int n, double deltaTime) {
        // !! Mover a POINT
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i){
            points[i].x += points[i].vx * deltaTime;
            points[i].y += points[i].vy * deltaTime;
        }
    }

    void reset_forces(Point* points, int n) {
        // !! Mover a POINT
        // #pragma omp parallel for
        for (int i = 0; i < n; ++i){
            points[i].fx = 0.0;
            points[i].fy = 0.0;
            points[i].fz = 0.0;
        }
    }

    void clear() {
        root->clear();
    }
};

#endif