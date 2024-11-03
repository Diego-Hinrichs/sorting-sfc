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

    void calculate_force(Point* points, int point_index, float& fx, float& fy, float softening_factor, float THETA, float G) {
        root->calculate_force_node(points, point_index, fx, fy, softening_factor, THETA, G);
    }
    
    // TODO: modificar para 3D
    void update_point(Point& p, float fx, float fy, float deltaTime) {
        float ax = fx / p.mass;
        float ay = fy / p.mass;

        p.vx += ax * deltaTime;
        p.vy += ay * deltaTime;

        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
    }

    void clear() {
        root->clear();
    }
};

#endif