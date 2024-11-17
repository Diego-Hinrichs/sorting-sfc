#include "qtnode.hpp"
#include "point.hpp"
#include <cmath>
#include <iostream>

QuadTreeNode::QuadTreeNode(Quad boundary, int capactiy)
    : boundary_(boundary), capacity_(capactiy), num_stored_points_(0), divided_(false), total_mass_(0.0), center_x_(0.0), center_y_(0.0) {
    point_indices_ = new int[capactiy];
    for (int i = 0; i < 4; i++) {
        children_[i] = nullptr;
    }
}

bool QuadTreeNode::insert(Point* points, int point_index) {
    Point& p = points[point_index];

    if (!boundary_.contains(p)) { return false; }

    if (num_stored_points_ < capacity_ && !divided_) {
        point_indices_[num_stored_points_++] = point_index;
        update_center_of_mass(p);
        return true;
    }

    // no space to store the point
    if (!divided_) {
        subdivide(points);
        recalculate_center_of_mass(points);
    }

    // finally insert the new point
    for (int j = 0; j < 4; ++j) {
        if (children_[j]->boundary_.contains(p)) {
            children_[j]->insert(points, point_index);
            update_center_of_mass(p);
            return true;
        }
    }

    std::cerr << "Error: No se pudo insertar el nuevo punto " << point_index << " en los hijos.\n";
    return false;
}

void QuadTreeNode::subdivide(Point* points) {
    for (int i = 0; i < 4; ++i) {
        children_[i] = new QuadTreeNode(boundary_.get_sub_quad(i), capacity_);
    }

    divided_ = true;
    // move the currents points to the children
    for (int i = 0; i < num_stored_points_; ++i) {
        int stored_index = point_indices_[i];
        for (int j = 0; j < 4; ++j) {
            if (children_[j]->boundary_.contains(points[stored_index])) {
                children_[j]->insert(points, stored_index);
                break;
            }
        }
    }

    num_stored_points_ = 0;
}

void QuadTreeNode::recalculate_center_of_mass(Point* points) {
    total_mass_ = 0.0;
    center_x_ = 0.0;
    center_y_ = 0.0;

    if (!divided_) {
        for (int i = 0; i < num_stored_points_; ++i) {
            Point& p = points[point_indices_[i]];
            total_mass_ += p.mass;
            center_x_ += p.x * p.mass;
            center_y_ += p.y * p.mass;
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            if (children_[i]) {
                total_mass_ += children_[i]->total_mass_;
                center_x_ += children_[i]->center_x_ * children_[i]->total_mass_;
                center_y_ += children_[i]->center_y_ * children_[i]->total_mass_;
            }
        }
    }

    if (total_mass_ > 0.0) {
        center_x_ /= total_mass_;
        center_y_ /= total_mass_;
    }
}

void QuadTreeNode::update_center_of_mass(const Point &point){
    double new_mass = total_mass_ + point.mass;
    center_x_ = (center_x_ * total_mass_ + point.x * point.mass) / new_mass;
    center_y_ = (center_y_ * total_mass_ + point.y * point.mass) / new_mass;
    total_mass_ = new_mass;
}

void QuadTreeNode::calculate_force_node(Point* points, int point_index, double softening_factor, double THETA, double G) {
    Point& p = points[point_index];

    // Si no hay puntos y no está dividido, no hacer nada
    if (num_stored_points_ == 0 && !divided_) {
        return;
    }

    if (!divided_) {
        for (int i = 0; i < num_stored_points_; ++i) {
            if (point_indices_[i] != point_index) {
                Point& _point = points[point_indices_[i]];
                p.add_force(_point, softening_factor, G);
            }
        }
        return;
    }

    // Calcular el tamaño del nodo y la distancia al centro de masa
    double s = boundary_.w;
    double dx = center_x_ - p.x;
    double dy = center_y_ - p.y;
    double dist_sq = dx * dx + dy * dy + softening_factor * softening_factor;
    double dist = sqrt(dist_sq);

    // Si (s / dist) < THETA, usar el centro de masa para calcular la fuerza
    if ((s / dist) < THETA) {
        double inv_dist = 1.0 / dist;
        double F = G * total_mass_ * inv_dist * inv_dist;
        p.fx += F * dx;
        p.fy += F * dy;
        return;
    }

    // Si no, calcular la fuerza recursivamente con los hijos
    for (int i = 0; i < 4; ++i) {
        if (children_[i]) {
            children_[i]->calculate_force_node(points, point_index, softening_factor, THETA, G);
        }
    }
}


void QuadTreeNode::clear() {
    num_stored_points_ = 0;
    total_mass_ = 0.0;
    center_x_ = 0.0;
    center_y_ = 0.0;
    delete[] point_indices_;
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
        if (children_[i]) {
            delete children_[i];
        }
    }
}
