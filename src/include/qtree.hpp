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

    void insert(Point* points, int point_index) {
        root->insert(points, point_index);
    }

    void calculate_force(Point* points, int point_index, double& fx, double& fy, double softening_factor, double THETA, double G) {
        root->calculate_force_node(points, point_index, fx, fy, softening_factor, THETA, G);
    }
    
    // TODO: modificar para 3D
    void update_point(Point& p, double fx, double fy, double deltaTime) {
        p.vx += p.fx * deltaTime;
        p.vy += p.fy * deltaTime;

        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
    }

    void clear() {
        root->clear();
    }
};

#endif