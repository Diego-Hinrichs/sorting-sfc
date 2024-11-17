#pragma once
#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include "point.hpp"
#include "quad.hpp"
#include "qtnode.hpp"
struct Mesh
{
public:
    Mesh();
    void createMesh(int dimension, int n);
    void createQuadMesh();
    void renderMesh(Point* points, int dimension, int n);
    void renderQuad(const Quad& quad);
    void renderQuadTree(QuadTreeNode* node);
    void drawMesh(int n);
    void clearMesh(); // delete from GPU
    ~Mesh();

private:
    GLuint VAO, VBO, quadVBO, quadVAO;
};

#endif