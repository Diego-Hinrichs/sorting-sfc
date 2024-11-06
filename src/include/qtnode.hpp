#ifndef QTNODE_H
#define QTNODE_H
#include "quad.hpp"

struct QuadTreeNode {
public:
    QuadTreeNode(Quad boundary, int capacity);
    bool insert(Point* points, int point_index);
    void subdivide(Point* points);
    void calculate_force_node(Point* points, int point_index, double softening_factor, double THETA, double G);
    void clear();
    ~QuadTreeNode();

private:
    Quad boundary_;
    QuadTreeNode* children_[4]; // TODO: std::unique_ptr
    int capacity_;
    int num_stored_points_;
    int* point_indices_;
    bool divided_;
    bool is_external_ = false;
    double total_mass_;
    double center_x_, center_y_;
};

#endif