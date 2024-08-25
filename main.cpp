#include <iostream>
#include <random>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <ctime>
#include <chrono>
#include <string.h>
#include "utils.h"

using namespace std::chrono;

float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraSpeed = 10.0f; // Velocidad de la cámara
float zoomLevel = 1.0f; // Nivel de zoom inicial

void processInput(GLFWwindow *window);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// N-Body Barnes-Hut
const float THETA = 0.5; // Apertura
Body *bodies;

struct Quad {
    float x, y; // Centro cuadrante
    float w, h; // Tamaño (x + w, y + h) desde el centro a los bordes

    Quad(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

    //Función que verifica si un cuerpo está dentro del cuadrante
    bool contains(const Body &body) const {
        float left = x - w / 2;
        float right = x + w / 2;
        float top = y + h / 2;
        float bottom = y - h / 2;
        return (body.x >= left && body.x <= right && body.y <= top && body.y >= bottom);
    }
    
    //Función que verifica si un cuadrante se intersecta con otro (para query, quad es un rectangulo, definido por el usuario)
    bool intersects(const Quad &range) const {
        return (
            x - w / 2 <= range.x + range.w / 2 &&
            x + w / 2 >= range.x - range.w / 2 &&
            y - h / 2 <= range.y + range.h / 2 &&
            y + h / 2 >= range.y - range.h / 2
        );
    }
    void draw() const {
        glBegin(GL_LINE_LOOP);
        glVertex2f(x - w / 2, y - h / 2);
        glVertex2f(x + w / 2, y - h / 2);
        glVertex2f(x + w / 2, y + h / 2);
        glVertex2f(x - w / 2, y + h / 2);
        glEnd();
    }
};

struct QuadTree {
    int capacity;
    int size = 0;
    
    float mass = 0; // Total mass in this QuadTree node
    float comX = 0, comY = 0; // Center of mass
    
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
    
    void clear() {
        for (int i = 0; i < size; ++i) {
            //bodies[i] = Body(); // Reiniciar el elemento usando el constructor predeterminado
            indices[i] = -1;    // Clear indices
        }
        size = 0; // Reiniciar el tamaño del array
    }

    void subdivide() {
        float x = boundary.x;
        float y = boundary.y;
        float w = boundary.w / 2;
        float h = boundary.h / 2;

        topLeft = new QuadTree(Quad(x - w / 2, y + h / 2, w, h), capacity);
        topRight = new QuadTree(Quad(x + w / 2, y + h / 2, w, h), capacity);
        bottomLeft = new QuadTree(Quad(x - w / 2, y - h / 2, w, h), capacity);
        bottomRight = new QuadTree(Quad(x + w / 2, y - h / 2, w, h), capacity);

        isDivided = true;
    }

    void insert(Body &body, int index) {
        if (!boundary.contains(body)) return;

        if (size < capacity) {
            //bodies[size] = body;
            indices[size] = index; // acceder a arreglo global
            size++;
            updateCenterOfMass(body);
            return;
        }
        
        if (!isDivided) {
            subdivide();
        }

        for(int i = 0; i < size; ++i){
            int idx = indices[i];
            Body b = bodies[idx];
            if (topLeft->boundary.contains(b)) {
                topLeft->insert(b, idx);
            } else if (topRight->boundary.contains(b)) {
                topRight->insert(b, idx);
            } else if (bottomLeft->boundary.contains(b)) {
                bottomLeft->insert(b, idx);
            } else if (bottomRight->boundary.contains(b)) {
                bottomRight->insert(b, idx);
            }
        }

        clear();

        // Insertar el nuevo cuerpo
        if (topLeft->boundary.contains(body)) {
            topLeft->insert(body, index);
        } else if (topRight->boundary.contains(body)) {
            topRight->insert(body, index);
        } else if (bottomLeft->boundary.contains(body)) {
            bottomLeft->insert(body, index);
        } else if (bottomRight->boundary.contains(body)) {
            bottomRight->insert(body, index);
        }
        updateCenterOfMass(body);
    }

    // ! Centro de masa del cuadrante, pasar bodies como argumento
    // cuadrante
    void updateCenterOfMass(const Body &body) {
        float totalMass = mass + body.mass;
        comX = (comX * mass + body.x * body.mass) / totalMass;
        comY = (comY * mass + body.y * body.mass) / totalMass;
        mass = totalMass;
    }

    // Fuerza que actua sobre un cuerpo, calculada con Barnes-Hut
    
    void computeForce(Body &body) const {
        /* if (size == 1 && &body != &bodies[0]) {
            body.addForce(bodies[0]);
        } */ 
        if (size <= 64) {
            // se recorre el arreglo de indices
            for(int i = 0; i < size; i++) {
                if(&body != &bodies[indices[i]]){
                    body.addForce(bodies[indices[i]]);
                }
            }
        } else if (isDivided) {
            float s = boundary.w; // tamaño ancho
            // Distancia entre el cuerpo y el centro de masa del cuadrante
            float d = std::sqrt((comX - body.x) * (comX - body.x) + (comY - body.y) * (comY - body.y));
            // quitar el false dsps
            if (false && (s / d) < THETA) { // factor de barnes
                Body comBody(comX, comY, 0, 0, 0, 0, mass); // centro del cuadrante 
                body.addForce(comBody);
            } else {
                topLeft->computeForce(body);
                topRight->computeForce(body);
                bottomLeft->computeForce(body);
                bottomRight->computeForce(body);
            }
        }
    }

    // funcion recursiva que recorra todo el arbol en prefijo
    void draw() const {
        boundary.draw();
        if (isDivided) {
            topLeft->draw();
            topRight->draw();
            bottomLeft->draw();
            bottomRight->draw();
        }
        for (int i = 0; i < size; ++i) {  // size es el número de cuerpos en este nodo del QuadTree
            const Body &b = bodies[indices[i]];     // Acceder a cada cuerpo en el arreglo plano
            glBegin(GL_POINTS);
            glVertex2f(b.x, b.y);
            glEnd();
            glRasterPos2f(b.x, b.y);
        }
    }
};

void updateForce(Body *bodies, int n, float G = 1.0f, float dt = 0.016f, float softeningFactor = 1e-10f, float velocityDamping = 1.0f) {    
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        
        float fx = 0.0f;
        float fy = 0.0f;

        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;

            float distSq = dx*dx + dy*dy;
            distSq += softeningFactor;
            float distSq3 = distSq * distSq;

            float dist = sqrt(distSq3);
            float invDist = 1.0f / dist;

            float F = G * bodies[j].mass * invDist;
 
            // A = F / m_i
            fx += F * dx;
            fy += F * dy; 
        }
        // V = A * dt
        bodies[i].vx += fx * dt * velocityDamping;
        bodies[i].vy += fy * dt * velocityDamping;
    }
    
}

void updatePosition(Body *bodies, int n, float dt) {
    // X = V * dt
    #pragma omp parallel for
    for(int i = 0; i < n; ++i){
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt; 
    }
}

QuadTree *quadTree;

// ver como recorrer el arbol y los vecinos
void simulateBH(float dt, int numBodies){
    for(int i = 0; i < numBodies; ++i){
        bodies[i].resetForce();
    }
    // Esta ordenando en este paso
    for(int i = 0; i < numBodies; ++i){
        quadTree->computeForce(bodies[i]);
    }
    
    for(int i = 0; i < numBodies; ++i){
        bodies[i].update(dt);
    }
}

void drawQuadTree(const QuadTree &qtree) {
    qtree.draw();
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <n> <alg [0|1]> <order [0|1|2]> <visualize [0|1]> <seed>\n", argv[0]);
        return 1;
    }

    int numBodies = atoi(argv[1]);
    unsigned int alg = atoi(argv[2]);
    unsigned int order = atoi(argv[3]);
    unsigned int visualize = atoi(argv[4]);
    unsigned int seed = atoi(argv[5]);
    
    bodies = (Body*)malloc(numBodies * sizeof(Body));
    initBodies(bodies, numBodies, seed);
    // Ordenar los cuerpos y los índices según Morton o Hilbert
    if (order == 1) {
        sortBodiesZOrder(bodies, numBodies);
    } else if (order == 2) {
        sortBodiesHilbert(bodies, numBodies, 32);
    }

    if (visualize) {
        if (!glfwInit()) {
            return -1;
        }

        int count;
        GLFWmonitor **monitors = glfwGetMonitors(&count);
        if (monitors == nullptr) {
            return -1;
        }

        GLFWmonitor *monitor = count > 1 ? monitors[1] : monitors[0];
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "2D N-Body Simulation", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);
        glfwSetScrollCallback(window, scrollCallback);

        glViewport(0, 0, mode->width, mode->height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-150, 150, -150, 150, -1, 1);
        glPointSize(1);
        glLineWidth(0.5f);
        
        Quad boundary(0, 0, 3500, 3500);
        if (alg == 1) {
            quadTree = new QuadTree(boundary, 64);
        }

        int step = 0;
        while (!glfwWindowShouldClose(window) && step < 3000) {
            glClear(GL_COLOR_BUFFER_BIT);
            processInput(window);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            float zoomedWidth = 150 * zoomLevel;
            float zoomedHeight = 150 * zoomLevel;
            glOrtho(-zoomedWidth + cameraX, zoomedWidth + cameraX, -zoomedHeight + cameraY, zoomedHeight + cameraY, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            if (alg == 1) {
                simulateBH(0.016, numBodies);
                delete quadTree;
                quadTree = new QuadTree(boundary, 64);

                if (order == 1) {
                    sortBodiesZOrder(bodies, numBodies);
                } else if (order == 2) {
                    sortBodiesHilbert(bodies, numBodies, 32);
                }

                for (int i = 0; i < numBodies; ++i) {
                    quadTree->insert(bodies[i], i);
                }

                drawQuadTree(*quadTree);
            } else if (alg == 0) {
                updateForce(bodies, numBodies, 1.0f, 0.016f);
                updatePosition(bodies, numBodies, 0.016f);

                // Dibujar los cuerpos directamente
                glBegin(GL_POINTS);
                for (int i = 0; i < numBodies; ++i) {
                    glVertex2f(bodies[i].x, bodies[i].y);
                }
                glEnd();
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
            step++;
        }

        glfwTerminate();
        if (alg == 1) {
            delete quadTree;
        }
        free(bodies);
        return 0;
    } else {
        // si es barnes-hut
        if (alg == 1) {
            Quad boundary(0, 0, 3500, 3500);
            quadTree = new QuadTree(boundary, 64);

            for (int i = 0; i < numBodies; ++i) {
                quadTree->insert(bodies[i], i);
            }

            int step = 0;
            while (step < 3000) {
                simulateBH(0.016, numBodies);
                delete quadTree;
                quadTree = new QuadTree(boundary, 64);

                for (int i = 0; i < numBodies; ++i) {
                    quadTree->insert(bodies[i], i);
                }
                step++;
            }
            delete quadTree;
        } else if (alg == 0) {
            int step = 0;
            while (step < 3000) {
                updateForce(bodies, numBodies, 1.0f, 0.016f);
                updatePosition(bodies, numBodies, 0.016f);
                step++;
            }
        }
        free(bodies);
        return 0;
    }
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraY += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraY -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraX -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraX += cameraSpeed;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    float zoomSensitivity = 0.1f; // Sensibilidad del zoom
    zoomLevel += yoffset * zoomSensitivity;
    if (zoomLevel < 0.1f) zoomLevel = 0.1f; // Evitar zoom inverso o demasiado cercano
}
