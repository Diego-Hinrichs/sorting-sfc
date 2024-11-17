#ifndef QTREE_H
#define QTREE_H
#include "qtnode.hpp"
#include "utils.hpp"

class QuadTree {
public:
    QuadTreeNode* root;
    int capacity;
    int n;
    double G;
    double dt;
    double softening_factor;

    QuadTree(int n, double G, double delta_time, double softening_factor, int capacity, Quad boundary)
        : capacity(capacity), n(n), G(G), dt(delta_time), softening_factor(softening_factor) {
        root = new QuadTreeNode(boundary, capacity);
    }

    ~QuadTree() {
        delete root;
    }

    void insert(Point* points) {
        for (int i = 0; i < n; ++i) {
            root->insert(points, i);
        }
    }

    void update_force(Point* points, double THETA) {
        for (int i = 0; i < n; ++i) {
            root->calculate_force_node(points, i, softening_factor, THETA, G);
        }
    }

    void simulate_bh(Point* points, double THETA){
        reset_forces(points, n);
        update_force(points, THETA);
        update_position(points, n, dt);
        clear();
    }

    void clear() {
        root->clear();
    }
};

#endif
