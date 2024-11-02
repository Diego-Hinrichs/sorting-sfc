#ifndef QTNODE_H
#define QTNODE_H
#include "quad.hpp"

struct QuadTreeNode {
public:
    QuadTreeNode(Quad boundary, int capacity);
    bool insert(Point* points, int point_index);
    void subdivide(Point* points);
    void calculate_force_node(Point* points, int point_index, float& fx, float& fy, float softening_factor, float THETA, float G);
    void clear();
    ~QuadTreeNode();

private:
    Quad boundary_;
    int capacity_;
    int* point_indices_;
    int num_stored_points_;
    bool divided_;
    QuadTreeNode* children_[4]; // TODO: std::unique_ptr
    float total_mass_;
    float center_x_, center_y_;
};

#endif