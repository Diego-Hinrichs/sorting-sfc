#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <chrono>
#include <math.h>
#include "window.hpp"
#include "qtree.hpp"
#include "nbody.hpp"

Point* points;
Window* mainWindow;

float half = 500; // mitad de tamano incial cuadrante -> 1000 de extremo a extremo
float THETA = 0.4f;

float G = 0.0009f;
float delta_time = 0.036f;
int max_capacity = 16;
float softening_factor = 0.000009;
int n;

// window
const int WIDTH = 800;
const int HEIGHT = 800;
GLuint VAO, VBO;

void initialize_points(Point* points, int n) {
    for (int i = 0; i < n; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * 20.0f - 10.0f;
        float y = static_cast<float>(rand()) / RAND_MAX * 20.0f - 10.0f;
        float z = 0.0f;
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
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(float) * 2, nullptr, GL_DYNAMIC_DRAW);

    // Configurar los atributos de vértice
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desvincular VAO y VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void updateVBO() {
    float* vertices = new float[n * 2];
    for (int i = 0; i < n; ++i) {
        vertices[i * 2 + 0] = points[i].x;
        vertices[i * 2 + 1] = points[i].y;
        // TODO: ADD Z same in initialize_points
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, n * sizeof(float) * 2, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] vertices;
}

void drawPoints() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, n);
    glBindVertexArray(0);
}

void update_point(Point& p, float fx, float fy, float deltaTime) {
    float ax = fx / p.mass;
    float ay = fy / p.mass;

    p.vx += ax * deltaTime;
    p.vy += ay * deltaTime;

    p.x += p.vx * deltaTime;
    p.y += p.vy * deltaTime;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " (numero de puntos) (numero de pasos de tiempo) (alg)" << std::endl;
        std::cout << "alg = 1: Fuerza Bruta" << std::endl;
        std::cout << "alg = 2: Barnes Hut" << std::endl; 
        std::cout << "alg = 3: Modo DEBUG entre ambos" << std::endl; 
        return 1;
    }

    n = std::atoi(argv[1]);
    int k = std::atoi(argv[2]);
    int alg = std::atoi(argv[3]);

    NBody* nbody;
    QuadTree* quadTree;

    if (alg == 1) { 
        points = (Point*)malloc(n * sizeof(Point));
        initialize_points(points, n);
    
        // !! SETUP FB
        nbody = new NBody(n, G, delta_time, softening_factor);

    } else if (alg == 2) { 
        points = (Point*)malloc(n * sizeof(Point));
        initialize_points(points, n);

        // !! SETUP BH
        Quad rootBoundary(0, 0, half, half);
        quadTree = new QuadTree(rootBoundary, max_capacity);

    } else if (alg > 2){
        std::cout << "Modo DEBUG" << std::endl;
        Point* fb = (Point*)malloc(n * sizeof(Point));
        Point* bh = (Point*)malloc(n * sizeof(Point));

        std::cout << "Generando puntos" << std::endl;
        initialize_points(fb, n);
        for(int i = 0; i < n; ++i){
            bh[i] = fb[i];
        }

        std::cout << "Setup de FB y BH" << std::endl;
        nbody = new NBody(n, G, delta_time, softening_factor);
        Quad rootBoundary(0, 0, half, half);
        quadTree = new QuadTree(rootBoundary, max_capacity);

        std::cout << "Simulando" << std::endl;
        int step = 0;
        while(step < k){
            // !! FUERZA BRUTA
            nbody->simulateFB(fb);

            // !! BARNES HUT
            quadTree->clear();
            for (int i = 0; i < n; ++i) {
                quadTree->insert(bh, i);
            }

            #pragma omp parallel for
            for (int i = 0; i < n; ++i) { // !! calcular fuerzas y actualizar puntos
                float fx = 0.0f;
                float fy = 0.0f;
                quadTree->calculate_force(bh, i, fx, fy, softening_factor, THETA, G);
                update_point(bh[i], fx, fy, delta_time); // sumatoria de velocidades
            }

            // TODO: Calcular errores para poder parametrizar

            step++;
        };
        std::cout << "Simulacion terminada" << std::endl;
        
        delete[] points;
        delete[] nbody;
        delete[] quadTree;
        return 0;
    }

    // !! iniciar window, VAO y VBO
    mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->init(); 
    setupVAOandVBO();
    
    // !! temporizador para desacoplar render y simulación
    auto lastTime = std::chrono::high_resolution_clock::now();
    int step = 0;
    
    // TODO: WHEN `ESC` OR `X` IS PRESSED (CORE DUMPED)
    while (!mainWindow->getShouldClose()) {

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;

        // Actualizar la simulación cada cierto tiempo (cada 0.016s para ~60 FPS)
        if (elapsed.count() > 0.016f) {
            if (alg == 1) {
                nbody->simulateFB(points);

            } else if (alg == 2){
                quadTree->clear();
                for (int i = 0; i < n; ++i) {
                    quadTree->insert(points, i); // !! insertar el índice del punto
                }

                #pragma omp parallel for
                for (int i = 0; i < n; ++i) { // !! calcular fuerzas y actualizar puntos
                    float fx = 0.0f;
                    float fy = 0.0f;
                    quadTree->calculate_force(points, i, fx, fy, softening_factor, THETA, G);
                    update_point(points[i], fx, fy, delta_time);
                }
            }
            // !! Actualizar VBO con las nuevas posiciones
            updateVBO();
            step++;
            lastTime = currentTime;
        }

        // Renderización con OpenGL
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawPoints();
        mainWindow->swapBuffers();
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // BUG: Debe ser que intento borrar uno que no se inicializo, solo se declaro (?
    delete[] points;
    delete[] nbody;
    delete[] quadTree;
    return 0;
}
