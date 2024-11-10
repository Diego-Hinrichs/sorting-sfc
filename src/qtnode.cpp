#include "qtnode.hpp"
#include "point.hpp"
#include <cmath> // sqrt
#include <iostream>

// constuctor
QuadTreeNode::QuadTreeNode(Quad boundary, int capacity)
    : boundary_(boundary), capacity_(capacity), num_stored_points_(0), divided_(false), total_mass_(0.0), center_x_(0.0), center_y_(0.0) {
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

    if (num_stored_points_ < capacity_ && !divided_) {
        point_indices_[num_stored_points_++] = point_index; // save the idx

        double new_mass = total_mass_ + p.mass;
        
        center_x_ = (center_x_ * total_mass_ + p.x * p.mass) / new_mass;
        center_y_ = (center_y_ * total_mass_ + p.y * p.mass) / new_mass;
        
        total_mass_ = new_mass;
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

    return false; 
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

void QuadTreeNode::calculate_force_node(Point* points, int point_index, double softening_factor, double THETA, double G) {
    Point& p = points[point_index];
    
    // punto == nodo
    if (num_stored_points_ == 0 && !divided_) {
        return;
    }
    
    // Si el nodo es una hoja y contiene un punto diferente al actual
    if (!divided_) {
        if (num_stored_points_ == 1 && point_indices_[0] != point_index) {
            Point& _point = points[point_indices_[0]];
            p.add_force(_point, softening_factor, G);
        }
        return;
    }
    
    // if (num_stored_points_ == 1 && point_indices_[0] != point_index) {
    //     // std::cout<< "Point[" << point_index << "] updated" << std::endl;
    //     Point& _point = points[point_indices_[0]];
    //     p.add_force(_point, softening_factor, G);
    // } 
    
    double s = boundary_.half_width;
    double dx = center_x_ - p.x;
    double dy = center_y_ - p.y;
    double dist_sq = dx * dx + dy * dy + softening_factor;
    double dist = sqrt(dist_sq);
    double inv_dist = 1.0 / dist;
    
    if ((s / dist) < THETA) {
        std::cout<< "(s/d): " << s / dist << std::endl;
        double F = G * total_mass_ * inv_dist * inv_dist;
        p.fx += F * dx;
        p.fy += F * dy;

    } else if (divided_) {
        for (int i = 0; i < 4; ++i) {
            if (children_[i]) {
                children_[i]->calculate_force_node(points, point_index, softening_factor, THETA, G);
            }
        }
    }
}

void QuadTreeNode::clear() {
    num_stored_points_ = 0;
    total_mass_ = 0.0;
    center_x_ = 0.0;
    center_y_ = 0.0;
    point_indices_ = new int[capacity_];
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

