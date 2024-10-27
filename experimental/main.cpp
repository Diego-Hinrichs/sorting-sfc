#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <omp.h>
#include <chrono>

// constantes sim
float half = 500; // mitad de tamano incial cuadrante -> 1000 de extremo a extremo
float deltaTime = 0.036f;
const float G = 0.09f;
const float THETA = 0.2f;

// TODO: implementar dona

// window
const int WIDTH = 800;
const int HEIGHT = 800;

class Point {
public:
    float x, y, z;     // Posición
    float vx, vy, vz;  // Velocidad
    float mass;        // Masa

    Point(float x, float y, float z, float mass)
        : x(x), y(y), z(z), vx(0.0f), vy(0.0f), vz(0.0f), mass(mass) {}

    Point() : x(0), y(0), z(0), vy(0.0f), vz(0.0f), mass(1.0f) {}
};

// Arreglo global de puntos
Point* points;
int numPoints;

// Variables para VAO y VBO
GLuint VAO, VBO;

void initializePoints(int numPoints) {
    points = (Point*)malloc(numPoints * sizeof(Point));
    for (int i = 0; i < numPoints; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * 20.0f - 10.0f;
        float y = static_cast<float>(rand()) / RAND_MAX * 20.0f - 10.0f;
        float z = static_cast<float>(rand()) / RAND_MAX * 20.0f - 10.0f;
        float mass = 1.0f;
        points[i] = Point(x, y, z, mass);
    }
}

void setupVAOandVBO() {
    // Generar VAO y VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Configurar VAO
    glBindVertexArray(VAO);

    // Configurar VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(float) * 2, nullptr, GL_DYNAMIC_DRAW);

    // Configurar los atributos de vértice
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desvincular VAO y VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void updateVBO() {
    float* vertices = new float[numPoints * 2];
    for (int i = 0; i < numPoints; ++i) {
        vertices[i * 2] = points[i].x;
        vertices[i * 2 + 1] = points[i].y;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numPoints * sizeof(float) * 2, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] vertices;
}

void drawPoints() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, numPoints);
    glBindVertexArray(0);
}

void updatePoint(Point& p, float fx, float fy, float deltaTime) {
    float ax = fx / p.mass;
    float ay = fy / p.mass;

    p.vx += ax * deltaTime;
    p.vy += ay * deltaTime;

    p.x += p.vx * deltaTime;
    p.y += p.vy * deltaTime;
}

class Quad {
public:
    float x, y;                     // Center of the quad
    float halfWidth, halfHeight;

    Quad(float x, float y, float halfWidth, float halfHeight)
        : x(x), y(y), halfWidth(halfWidth), halfHeight(halfHeight) {}

    // point is in the quad?
    bool contains(Point& p) {
        return (p.x >= x - halfWidth && p.x < x + halfWidth &&
                p.y >= y - halfHeight && p.y < y + halfHeight);
    }

    // Divide the quad in 4 subquads of the same size
    Quad getSubQuad(int quadrant) {
        float newHalfWidth = halfWidth / 2.0f;
        float newHalfHeight = halfHeight / 2.0f;

        switch (quadrant) {
            case 0: return Quad(x - newHalfWidth, y - newHalfHeight, newHalfWidth, newHalfHeight); // NW
            case 1: return Quad(x + newHalfWidth, y - newHalfHeight, newHalfWidth, newHalfHeight); // NE
            case 2: return Quad(x - newHalfWidth, y + newHalfHeight, newHalfWidth, newHalfHeight); // SW
            case 3: return Quad(x + newHalfWidth, y + newHalfHeight, newHalfWidth, newHalfHeight); // SE
            default: return *this; // Fallback
        }
    }
};

class QuadTreeNode {
public:
    Quad boundary;
    int capacity;
    int* pointIndices;  // Índices de puntos si es una hoja -> https://excalidraw.com/
    int numStoredPoints;
    bool divided;
    QuadTreeNode* children[4];
    float totalMass;
    float centerX, centerY;

    QuadTreeNode(Quad boundary, int capacity)
        : boundary(boundary), capacity(capacity), numStoredPoints(0), divided(false), totalMass(0), centerX(0), centerY(0) {
        pointIndices = new int[capacity];
        for (int i = 0; i < 4; i++) {
            children[i] = nullptr;
        }
    }

    ~QuadTreeNode() {
        delete[] pointIndices;
        for (int i = 0; i < 4; i++) {
            if (children[i]) delete children[i];
        }
    }

    bool insert(int pointIndex) {
        Point& p = points[pointIndex];

        if (!boundary.contains(p)) {
            return false;
        }

        // if the node has capacity then store the idx of the point. update the mass of the quad and the center of mass
        if (numStoredPoints < capacity && !divided) {
            pointIndices[numStoredPoints++] = pointIndex;
            totalMass += p.mass;
            centerX = (centerX * (totalMass - p.mass) + p.x * p.mass) / totalMass;
            centerY = (centerY * (totalMass - p.mass) + p.y * p.mass) / totalMass;
            return true;
        }

        // full then subvidide
        if (!divided) {
            subdivide();
        }

        // insert the point in one child
        for (int i = 0; i < 4; ++i) {
            if (children[i]->insert(pointIndex)) {
                return true;
            }
        }

        return false;  // this should not
    }

    void subdivide() {
        for (int i = 0; i < 4; ++i) {
            children[i] = new QuadTreeNode(boundary.getSubQuad(i), capacity);
        }
        divided = true;

        // insert point in the respectively child node, the same loop but for every single point in the node
        for (int i = 0; i < numStoredPoints; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (children[j]->insert(pointIndices[i])) {
                    break;
                }
            }
        }
        numStoredPoints = 0;  // no points
    }

    // calculate the force using BH
    void calculateForce(int pointIndex, float& fx, float& fy) {
        Point& p = points[pointIndex]; // point entering the quadrant
        
        // single point, no mass
        if (numStoredPoints == 0 && !divided) {
            return;
        }

        float dx = centerX - p.x;
        float dy = centerY - p.y;
        float dist = std::sqrt(dx * dx + dy * dy + 1e-5f); // 1e-5f softening factor

        // if (exists another point in the node and its not the same) or (the quad is divided and (s / d < theta))
        if ((numStoredPoints == 1 && pointIndices[0] != pointIndex) || (divided && (boundary.halfWidth / dist) < THETA)) {
            float force = (G * p.mass * totalMass) / (dist * dist);
            fx += force * (dx / dist);
            fy += force * (dy / dist);
        } else if (divided) { // recursively for every child node
            for (int i = 0; i < 4; ++i) {
                if (children[i]) {
                    children[i]->calculateForce(pointIndex, fx, fy);
                }
            }
        }
    }

    // clear all 
    void clear() {
        numStoredPoints = 0;
        totalMass = 0;
        centerX = 0;
        centerY = 0;
        if (divided) {
            for (int i = 0; i < 4; ++i) {
                if (children[i]) {
                    children[i]->clear();
                    delete children[i];
                    children[i] = nullptr;
                }
            }
            divided = false;
        }
    }
};

class QuadTree {
public:
    QuadTreeNode* root;
    int capacity;

    QuadTree(Quad boundary, int capacity)
        : capacity(capacity) {
        root = new QuadTreeNode(boundary, capacity);
    }

    ~QuadTree() {
        delete root;
    }

    void insert(int pointIndex) {
        root->insert(pointIndex);
    }

    void calculateForce(int pointIndex, float& fx, float& fy) {
        root->calculateForce(pointIndex, fx, fy);
    }

    void clear() {
        root->clear();
    }
};

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <numero de puntos> <numero de pasos de tiempo>" << std::endl;
        return 1;
    }

    numPoints = std::atoi(argv[1]);
    int k = std::atoi(argv[2]);

    if (numPoints <= 0 || k <= 0) {
        std::cerr << "El número de puntos y el número de pasos de tiempo deben ser mayores que cero." << std::endl;
        return 1;
    }

    // Inicializar los puntos
    initializePoints(numPoints);

    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "No se pudo inicializar GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Barnes-Hut Simulation with VAO/VBO", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "No se pudo inicializar GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glOrtho(-20, 20, -20, 20, -1, 1); // Ajustar la escala para visualizar todos los puntos

    // Configurar VAO y VBO
    setupVAOandVBO();

    Quad rootBoundary(0, 0, half, half);
    QuadTree quadTree(rootBoundary, 1);

    // Temporizador para desacoplar render y simulación
    auto lastTime = std::chrono::high_resolution_clock::now();

    // Bucle de simulación y renderización
    int step = 0;
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;

        // Actualizar la simulación cada cierto tiempo (cada 0.016s para ~60 FPS)
        if (elapsed.count() > 0.016f) {
            quadTree.clear(); // Limpiar el QuadTree
            for (int i = 0; i < numPoints; ++i) {
                quadTree.insert(i); // Insertar el índice del punto
            }

            // Calcular fuerzas y actualizar puntos
            #pragma omp parallel for
            for (int i = 0; i < numPoints; ++i) {
                float fx = 0.0f;
                float fy = 0.0f;
                quadTree.calculateForce(i, fx, fy);
                updatePoint(points[i], fx, fy, deltaTime);
            }

            // Actualizar VBO con las nuevas posiciones
            updateVBO();

            // Incrementar el paso de simulación
            step++;
            lastTime = currentTime;
        }

        // Renderización con OpenGL
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawPoints();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    free(points);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}