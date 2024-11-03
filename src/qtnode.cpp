#include "qtnode.hpp"
#include "point.hpp"
#include <cmath> // sqrt

QuadTreeNode::QuadTreeNode(Quad boundary, int capacity)
    : boundary_(boundary), capacity_(capacity), num_stored_points_(0), divided_(false), total_mass_(0), center_x_(0), center_y_(0) {
    point_indices_ = new int[capacity_];
    for (int i = 0; i < 4; i++) {
        children_[i] = nullptr;
    }
}

bool QuadTreeNode::insert(Point* points, int point_index) {
    Point& p = points[point_index];

    if (!boundary_.contains(p)) {
        return false;
    }

    // if the node has capacity then store the idx of the point. update the mass of the quad and the center of mass
    if (num_stored_points_ < capacity_ && !divided_) {
        point_indices_[num_stored_points_++] = point_index;
        total_mass_ += p.mass;
        center_x_ = (center_x_ * (total_mass_ - p.mass) + p.x * p.mass) / total_mass_;
        center_y_ = (center_y_ * (total_mass_ - p.mass) + p.y * p.mass) / total_mass_;
        return true;
    }

    // full then subvidide
    if (!divided_) {
        subdivide(points);
    }

    // insert the point in one child
    for (int i = 0; i < 4; ++i) {
        if (children_[i]->insert(points, point_index)) {
            return true;
        }
    }

    return false;  // this should not
}

void QuadTreeNode::subdivide(Point* points) {
    for (int i = 0; i < 4; ++i) {
        children_[i] = new QuadTreeNode(boundary_.get_sub_quad(i), capacity_);
    }
    divided_ = true;

    // insert point in the respectively child node, the same loop but for every single point in the node
    for (int i = 0; i < num_stored_points_; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (children_[j]->insert(points, point_indices_[i])) {
                break;
            }
        }
    }
    num_stored_points_ = 0;  // no points
}

void QuadTreeNode::calculate_force_node(Point* points, int point_index, double& fx, double& fy, double softening_factor, double THETA, double G) {
    Point& p = points[point_index]; // point entering the quadrant
    
    // no point in the quad
    if (num_stored_points_ == 0 && !divided_) {
        return;
    }

    float dx = center_x_ - p.x;
    float dy = center_y_ - p.y;
    float dist = std::sqrt(dx * dx + dy * dy + softening_factor); // 1e-5f softening factor

    // if (exists another point in the node and its not the same) or (the quad is divided and (s / d < theta))
    if ((num_stored_points_ == 1 && point_indices_[0] != point_index) || (divided_ && (boundary_.half_width / dist) < THETA)) {
        float force = (G * total_mass_) / (dist * dist);
        fx += force * (dx / dist);
        fy += force * (dy / dist);
    } else if (divided_) { // recursively for every child node
        for (int i = 0; i < 4; ++i) {
            if (children_[i]) {
                children_[i]->calculate_force_node(points, point_index, fx, fy, softening_factor, THETA, G);
            }
        }
    }
}

void QuadTreeNode::clear() {
    num_stored_points_ = 0;
    total_mass_ = 0;
    center_x_ = 0;
    center_y_ = 0;
    if (divided_) {
        for (int i = 0; i < 4; ++i) {
            if (children_[i]) {
                children_[i]->clear();
                delete children_[i];
                children_[i] = nullptr;
            }
        }
        divided_ = false;
    }
}

QuadTreeNode::~QuadTreeNode() {
    delete[] point_indices_;
    for (int i = 0; i < 4; i++) {
        if (children_[i]) delete children_[i];
    }
}

