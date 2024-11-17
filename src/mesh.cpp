#include "mesh.hpp"
#include "quad.hpp"
#include "qtree.hpp"

Mesh::Mesh(){
    VAO = 0;
    VBO = 0;
    quadVAO = 0;
    quadVBO = 0;
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

void Mesh::createQuadMesh() {
    // Generar VAO y VBO para los cuadrantes
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    // Configurar VAO para los cuadrantes
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Configurar los atributos de vértice
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desvincular VAO y VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::renderQuad(const Quad& quad) {
    // Calcular las esquinas del cuadrante basado en el centro (x, y)
    float x1 = quad.x - quad.w; // Esquina izquierda
    float y1 = quad.y - quad.h; // Esquina inferior
    float x2 = quad.x + quad.w; // Esquina derecha
    float y2 = quad.y + quad.h; // Esquina superior

    // Definir los vértices del cuadrante
    float vertices[] = {
        x1, y1,  // Esquina inferior izquierda
        x2, y1,  // Esquina inferior derecha
        x2, y2,  // Esquina superior derecha
        x1, y2   // Esquina superior izquierda
    };

    // Actualizar el VBO con los vértices del cuadrante
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Dibujar el cuadrante usando GL_LINE_LOOP
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}


void Mesh::renderQuadTree(QuadTreeNode* node) {
    if (node == nullptr) return;

    // Renderizar el Quad del nodo actual
    renderQuad(node->boundary_);

    // Renderizar recursivamente los hijos del nodo actual
    for (int i = 0; i < 4; ++i) {
        if (node->children_[i] != nullptr) {
            renderQuadTree(node->children_[i]);
        }
    }
}

void Mesh::clearMesh(){
    if(VBO != 0){
        glDeleteBuffers(1, &VBO);
    }

    if(VAO != 0){
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    
    if(quadVAO != 0){
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }

    if(quadVBO != 0){
        glDeleteVertexArrays(1, &quadVBO);
        quadVBO = 0;
    }
}

Mesh::~Mesh(){
    clearMesh();
};