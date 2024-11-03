#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <chrono>
#include <math.h>

#include "window.hpp"
#include "mesh.hpp"
#include "qtree.hpp"
#include "nbody.hpp"

#define EPSILON 1e-4

Point* points;
int dimension = 2;

// Barnes-Hut params
float THETA = 0.4f;
float half = 500; // mitad de tamano incial cuadrante -> 1000 de extremo a extremo
int max_capacity = 64;

// NBody params
float G = 0.0009f;
float delta_time = 0.036f;
float softening_factor = 0.000009;
int n;

// window
Window* mainWindow;
const int WIDTH = 800;
const int HEIGHT = 800;

void initialize_points(Point* points, int n);
bool debug_mode(NBody* nbody, QuadTree* quadTree, int n, int k, int x);

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "\nUso: " << argv[0] << " (numero de puntos) (numero de pasos de tiempo) (alg)\n" << std::endl;
        std::cout << "alg = 1: Fuerza Bruta" << std::endl;
        std::cout << "alg = 2: Barnes Hut" << std::endl; 
        std::cout << "alg = 3: Modo DEBUG entre ambos\n" << std::endl; 
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

    } else if (alg > 2) {
        return debug_mode(nbody, quadTree, n, k, 100);
    }

    // !! iniciar window, VAO y VBO
    mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->init();
    Mesh *mesh = new Mesh();
    mesh->createMesh(dimension, n);
    
    // !! temporizador para desacoplar render y simulación
    auto lastTime = std::chrono::high_resolution_clock::now();
    int step = 0;

    while (!mainWindow->getShouldClose()) {

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;

        // !! actualizar la simulación cada cierto tiempo (cada 0.016s para ~60 FPS)
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
                    quadTree->update_point(points[i], fx, fy, delta_time);
                }
            }
            step++;
            lastTime = currentTime;
        }

        // !! render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh->renderMesh(points, dimension, n); // !! actualizar VBO
        mesh->drawMesh(n);                      // !! dibujar puntos en VAO
        mainWindow->swapBuffers();
        glfwPollEvents();
    }

    mesh->~Mesh(); // !! Borrar los buffers VBO y VAO
    
    free(points);
    delete nbody;
    delete quadTree;
    
    return 0;
}

bool debug_mode(NBody* nbody, QuadTree* quadTree, int n, int k, int x) {
    std::cout << "Modo DEBUG" << std::endl;
    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    std::cout << "Generando puntos" << std::endl;
    initialize_points(fb, n);
    for(int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    std::cout << "Setup de FB y BH" << std::endl;
    nbody = new NBody(n, G, delta_time, softening_factor);
    Quad rootBoundary(0, 0, half, half);
    quadTree = new QuadTree(rootBoundary, max_capacity);

    std::cout << "Simulando" << std::endl;
    int step = 0;
    while(step < k) {
        // !! FUERZA BRUTA
        nbody->simulateFB(fb);

        // !! BARNES HUT
        quadTree->clear();
        for (int i = 0; i < n; ++i) {
            quadTree->insert(bh, i);
        }

        #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            float fx = 0.0f;
            float fy = 0.0f;
            quadTree->calculate_force(bh, i, fx, fy, softening_factor, THETA, G);
            quadTree->update_point(bh[i], fx, fy, delta_time);
        }

        // Calcular errores cada X pasos
        if (step % x == 0) {
            double total_error = 0.0;
            #pragma omp parallel for reduction(+:total_error)
            for (int i = 0; i < n; ++i) {
                float dx = fb[i].x - bh[i].x;
                float dy = fb[i].y - bh[i].y;
                double distance_error = sqrt(dx * dx + dy * dy);
                total_error += distance_error;
            }
            double mean_error = total_error / n;

            std::cout << "Paso " << step << ": Error medio de posiciones (MAE) = " << mean_error << std::endl;

            if (mean_error > EPSILON) {
                std::cout << "Advertencia: El error en el paso " << step << " excede la tolerancia de EPSILON (" << EPSILON << ")" << std::endl;
            }
        }

        step++;
    };

    std::cout << "Simulacion terminada" << std::endl;
    
    free(fb);
    free(bh);
    delete nbody;
    delete quadTree;
    return true;
}

void initialize_points(Point* points, int n) {
    for (int i = 0; i < n; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        float y = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        float z = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        float mass = 1.0f;
        points[i] = Point(x, y, z, mass);
    }
}