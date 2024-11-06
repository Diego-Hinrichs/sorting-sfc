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
unsigned int seed = 12345;

// Barnes-Hut params
double THETA = 0;
double half = 2.0; // mitad de tamano incial cuadrante -> half*2 de extremo a extremo
int max_capacity = 1;

// NBody params
double G = 1e-6;
double delta_time = 16e-2;
double softening_factor = 1e-15;
int n;

// window
Window* mainWindow;
const int WIDTH = 800;
const int HEIGHT = 800;
const char* title;

void initialize_points(Point* points, int n, unsigned int seed);
bool debug_mode(NBody* nbody, QuadTree* quadTree, int n, int k, int x);
void print_arrays(Point* fb, Point* bh, int n, int k);

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
    Quad rootBoundary(0, 0, half, half);

    if (alg == 1) { 
        points = (Point*)malloc(n * sizeof(Point));
        initialize_points(points, n, seed);
    
        // !! SETUP FB
        nbody = new NBody(n, G, delta_time, softening_factor, rootBoundary);
        title = "Brute-Force Simulation";

    } else if (alg == 2) { 
        points = (Point*)malloc(n * sizeof(Point));
        initialize_points(points, n, seed);

        // !! SETUP BH
        quadTree = new QuadTree(rootBoundary, max_capacity);
        title = "Barnes-Hut Simulation";

    } else if (alg > 2) {
        return debug_mode(nbody, quadTree, n, k, 100);
    }

    // !! iniciar window, VAO y VBO
    mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->init(title);
    Mesh *mesh = new Mesh();
    mesh->createMesh(dimension, n);
    
    // !! temporizador para desacoplar render y simulación
    auto lastTime = std::chrono::high_resolution_clock::now();

    int step = 0;
    while (!mainWindow->getShouldClose()) {
        // std::cout << "Step: [" << step << "]" << std::endl;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;

        // !! actualizar la simulación cada cierto tiempo (cada 0.016s para ~60 FPS)
        if (elapsed.count() > 0.016f) {
            if (alg == 1) {
                nbody->simulate_fb(points);
            } else if (alg == 2) {
                quadTree->clear();
                quadTree->insert(points, n);
                quadTree->calculate_force(points, n, softening_factor, THETA, G);
                quadTree->update_velocity(points, n, delta_time);
                quadTree->update_position(points, n, delta_time);
                quadTree->reset_forces(points, n);
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
    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    initialize_points(fb, n, seed);
    for(int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    Quad rootBoundary(0, 0, half, half);
    nbody = new NBody(n, G, delta_time, softening_factor, rootBoundary);
    quadTree = new QuadTree(rootBoundary, max_capacity);

    int step = 0;
    while(step < k) {
        // std::cout << "Step: [" << step << "]" << std::endl;
        // !! FUERZA BRUTA
        nbody->simulate_fb(fb);

        // !! BARNES HUT
        quadTree->clear();
        quadTree->insert(bh, n);
        quadTree->calculate_force(bh, n, softening_factor, THETA, G);
        quadTree->update_velocity(bh, n, delta_time);
        quadTree->update_position(bh, n, delta_time);
        quadTree->reset_forces(bh, n);

        // !! calcular errores cadax pasos
        // if (step % x == 0) {
        //     double total_error = 0.0;
        //     #pragma omp parallel for reduction(+:total_error)
        //     for (int i = 0; i < n; ++i) {
        //         double dx = fb[i].x - bh[i].x;
        //         double dy = fb[i].y - bh[i].y;
        //         double distance_error = sqrt(dx * dx + dy * dy);
        //         total_error += distance_error;
        //     }
        //     double mean_error = total_error / n;

        //     if (mean_error > EPSILON) {
        //         std::cout << "Advertencia: El error promedio (" << mean_error << ") en el paso [" << step << "] excede la tolerancia de EPSILON (" << EPSILON << ")" << std::endl;
        //     }

        //     printf("Step: %i\n", step);
        //     for (int i = 0; i < n; ++i) {
        //         printf("fb[%i] = (%10.7f, %10.7f)\t", i, fb[i].x, fb[i].y);
        //         printf("bh[%i] = (%10.7f, %10.7f)\n", i, bh[i].x, bh[i].y);
        //     }
        // }

        step++;
    };
    
    std::cout << "Simulacion terminada.\n" << std::endl;
    printf("Step: %i. Points: %i.\n", step, n);
    std::cout << "Posiciones finales:\n" << std::endl;
    for (int i = 0; i < 100; ++i) {
        printf("fb[%i] = (%10.7f, %10.7f)\t", i, fb[i].x, fb[i].y);
        printf("bh[%i] = (%10.7f, %10.7f)\n", i, bh[i].x, bh[i].y);
    }
    
    // print_arrays(fb, bh, n, k);

    free(fb);
    free(bh);
    delete nbody;
    delete quadTree;
    return true;
}

void initialize_points(Point* points, int n, unsigned int seed) {
    // Inicializar el generador de números aleatorios con la semilla
    srand(seed);

    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        double y = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        double z = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        double mass = 1.0;
        points[i] = Point(x, y, z, mass);
    }
};

// mejorar
void print_arrays(Point* fb, Point* bh, int n, int k) {
    if (n < 100) {
        std::cout << "fb_data = [";
        for (int i = 0; i < n; ++i) {
            if (i % 3 == 0) {
                std::cout << "\n\t";
            }
            printf("(%f, %f), ", fb[i].x, fb[i].y);
        }
        std::cout << "\n]" << std::endl;
        std::cout << "\nbh_data = [";
        for (int i = 0; i < n; ++i) {
            if (i % 3 == 0) {
                std::cout << "\n\t";
            }
            printf("(%f, %f), ", bh[i].x, bh[i].y);
        }
        std::cout << "\n]\n" << std::endl;
        std::cout << "n = " << n << std::endl;
        std::cout << "step = " << k << std::endl;
    } else {
        std::cout << "fb_data = [";
        for (int i = 0; i < 100; ++i) {
            if (i % 3 == 0) {
                std::cout << "\n\t";
            }
            printf("(%f, %f), ", fb[i].x, fb[i].y);
        }
        std::cout << "\n]" << std::endl;
        std::cout << "\nbh_data = [";
        for (int i = 0; i < 100; ++i) {
            if (i % 3 == 0) {
                std::cout << "\n\t";
            }
            printf("(%f, %f), ", bh[i].x, bh[i].y);
        }
        std::cout << "\n]\n" << std::endl;
        std::cout << "n = " << n << std::endl;
        std::cout << "step = " << k << std::endl;
    }
}