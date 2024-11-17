#ifndef QTNODE_H
#define QTNODE_H
#include "quad.hpp"

struct QuadTreeNode {
public:
    Quad boundary_;
    QuadTreeNode* children_[4];
    QuadTreeNode(Quad boundary, int capacity);
    bool insert(Point* points, int point_index);
    void subdivide(Point* points);
    void recalculate_center_of_mass(Point* points);
    void calculate_force_node(Point* points, int point_index, double softening_factor, double THETA, double G);
    void update_center_of_mass(const Point &point);
    void clear();
    ~QuadTreeNode();

private:
    double total_mass_;
    double center_x_, center_y_;
    bool divided_;
    int num_stored_points_;
    int capacity_;
    int* point_indices_;
};

#endif