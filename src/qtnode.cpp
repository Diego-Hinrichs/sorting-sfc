#include "qtnode.hpp"
#include "point.hpp"
#include <cmath> // sqrt
#include <iostream>

// constuctor
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

    // if the node has capacity then save the idx of the point. 
    // update the mass of the quad and the center of mass
    // if capacity == 1 then the point is the only one in the node
    // and the mass is the mass of the point, same thing with the center_x_y of mass p.x_y
    if (num_stored_points_ < capacity_ && !divided_) {
        point_indices_[num_stored_points_++] = point_index; // save the indice
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
        children_[i]->is_external_ = true;
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
    total_mass_ = 0.0;
    center_x_ = boundary_.half_width;
    center_y_ = boundary_.half_height;
}

void QuadTreeNode::calculate_force_node(Point* points, int point_index, double softening_factor, double THETA, double G) {
    Point& p = points[point_index];
    
    // punto == nodo
    if (num_stored_points_ == 0 && !divided_) {
        return;
    }


    
    if (num_stored_points_ == 1 && point_indices_[0] != point_index) {
        /*1. If the current node is an external node (and it is not body b), 
        calculate the force exerted by the current node on b, and add this amount to b’s net force.*/
        Point& _point = points[point_indices_[0]];
        double _dx = _point.x - p.x;
        double _dy = _point.y - p.y;
        double _dist_sq = _dx * _dx + _dy * _dy + softening_factor;
        double _dist = sqrt(_dist_sq);
        double _inv_dist = 1.0 / _dist;
        double F = G * _point.mass * _inv_dist * _inv_dist;
        p.fx += F * _dx;
        p.fy += F * _dy;

    // nodo interno
    } 
    
    double s = boundary_.half_width;
    double dx = center_x_ - p.x;
    double dy = center_y_ - p.y;
    double dist_sq = dx * dx + dy * dy + softening_factor;
    double dist = sqrt(dist_sq);
    double inv_dist = 1.0 / dist;
    
    if ((s / dist) < THETA) {
        /*2. Otherwise, calculate the ratio s/d. If s/d < θ, treat this internal node as a single body, 
        and calculate the force it exerts on body b, and add this amount to b’s net force. */
        // std::cout<< "(s/d): " << s / dist << std::endl;
        double F = G * total_mass_ * inv_dist * inv_dist;
        p.fx += F * dx;
        p.fy += F * dy;

    } else if (divided_) {
        /*3. Otherwise, run the procedure recursively on each of the current node’s children. */
        for (int i = 0; i < 4; ++i) {
            if (children_[i]) {
                children_[i]->calculate_force_node(points, point_index, softening_factor, THETA, G);
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

