#pragma once
#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include "point.hpp"

struct Mesh
{
public:
    Mesh();

    void createMesh(int dimension, int n);
    void renderMesh(Point* points, int dimension, int n);
    void drawMesh(int n);
    void clearMesh(); // delete from GPU
    ~Mesh();

private:
    GLuint VAO, VBO, IBO;
    GLsizei indexCount;

};

#endif