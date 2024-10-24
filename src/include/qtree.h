#ifndef QTREE_H
#define QTREE_H
#include "body.h"

struct Quad {
    float x, y;
    float w, h;

    Quad(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {};
    bool contains(const Body &body) const;
};

struct QuadTree {
    int capacity;
    int size = 0; // Cantidad de elementos en el cuadrante
    
    float mass = 0;             // Total mass in this QuadTree node
    float comX = 0, comY = 0;   // Center of mass
    
    bool isDivided = false;

    Quad boundary;
    // Body *bodies = (Body*)malloc(capacity * sizeof(Body));

    int *indices = (int*)malloc(capacity * sizeof(int));

    QuadTree *topLeft = nullptr;
    QuadTree *topRight = nullptr;
    QuadTree *bottomLeft = nullptr;
    QuadTree *bottomRight = nullptr;

    QuadTree(const Quad &boundary, int capacity)
        : boundary(boundary), capacity(capacity) {}
    
    ~QuadTree() {
        delete topLeft;
        delete topRight;
        delete bottomLeft;
        delete bottomRight;
        // free(bodies);
        free(indices);
    }

    void clear();
    void subdivide();
    void insert(Body &body, int index, Body *bodies);

    // ! Esto no es propio del QT
    void updateCenterOfMass(const Body &body);
    void computeForce(Body &body, float THETA, Body *bodies) const;
};

#endif