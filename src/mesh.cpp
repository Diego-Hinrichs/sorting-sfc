#include "mesh.hpp"

Mesh::Mesh(){
    VAO = 0;
    VBO = 0;
};

void Mesh::createMesh(int dimension, int n){
    // Generar VAO y VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Configurar VAO
    glBindVertexArray(VAO);

    // Configurar VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, dimension * n * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Configurar los atributos de vértice
    glVertexAttribPointer(0, dimension, GL_FLOAT, GL_FALSE, dimension * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desvincular VAO y VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
};

void Mesh::renderMesh(Point* points, int dimension, int n){
    float* vertices = new float[n * dimension];
    for (int i = 0; i < n; ++i) {
        vertices[i * dimension + 0] = points[i].x;
        vertices[i * dimension + 1] = points[i].y;
        if(dimension == 3) {
            vertices[i * dimension + 2] = points[i].z;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dimension * n * sizeof(float), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] vertices;
}

void Mesh::drawMesh(int n) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, n);
    glBindVertexArray(0);
}

void Mesh::clearMesh(){
    // index buffer object

    if(VBO != 0){
        glDeleteBuffers(1, &VBO);
        IBO = 0;
    }

    if(VAO != 0){
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

}

Mesh::~Mesh(){
    clearMesh();
};