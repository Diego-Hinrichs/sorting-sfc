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
double THETA = 0.0;
int max_capacity = 1;

// System
double G = 1e-5;
double delta_time = 1e-3;
double softening_factor = 1e-8;
double half = 10.0; // mitad de tamano incial cuadrante -> half*2 de extremo a extremo
int n;

// window
Window* mainWindow;
const int WIDTH = 800;
const int HEIGHT = 800;
const char* title;

void initialize_points(Point* points, int n, unsigned int seed);
bool debug_mode(int n, int k, int x);
void print_arrays(Point* fb, Point* bh, int n, int k);
void print_points(Point* fb, Point* bh, int n);
void calculate_error(Point *fb, Point *bh, int n);

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
    Quad rootBoundary(0.0, 0.0, half, half);

    if (alg == 1) { 
        points = (Point*)malloc(n * sizeof(Point));
        initialize_points(points, n, seed);
        nbody = new NBody(n, G, delta_time, softening_factor, rootBoundary);
        title = "Brute Force Simulation";

    } else if (alg == 2) { 
        points = (Point*)malloc(n * sizeof(Point));
        initialize_points(points, n, seed);
        quadTree = new QuadTree(n, G, delta_time, softening_factor, max_capacity, rootBoundary);
        title = "Barnes-Hut Simulation";

    } else if (alg > 2) {
        return debug_mode(n, k, 100);
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
                quadTree->insert(points);
                quadTree->update_force(points, THETA);
                quadTree->update_velocity(points);
                quadTree->update_position(points);
                quadTree->reset_forces(points);
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

bool debug_mode(int n, int k, int x) {
    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    initialize_points(fb, n, seed);
    for(int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    std::cout << "Simulacion iniciada.\n" << std::endl;
    printf("Step: %i. Points: %i.\n", 0, n);
    std::cout << "Posiciones inciales:\n" << std::endl;
    print_points(fb, bh, 10);

    Quad rootBoundary(0.0, 0.0, half, half);
    NBody* nbody = new NBody(n, G, delta_time, softening_factor, rootBoundary);
    QuadTree* quadTree = new QuadTree(n, G, delta_time, softening_factor, max_capacity, rootBoundary);
    
    // !! FUERZA BRUTA 
    int step = 0;
    while(step < k) {
        // std::cout << "Step: [" << step << "]" << std::endl;
        nbody->simulate_fb(fb);
        step++;
    };

    // !! BARNES HUT
    step = 0;
    while(step < k) {
        quadTree->clear();
        quadTree->insert(bh);
        quadTree->update_force(bh, THETA);
        quadTree->update_velocity(bh);
        quadTree->update_position(bh);
        quadTree->reset_forces(bh);
        step++;
    }

    std::cout << "Simulacion terminada.\n" << std::endl;
    printf("Step: %i. Points: %i.\n", step, n);
    std::cout << "Posiciones finales:\n" << std::endl;
    print_points(fb, bh, 10);
    std::cout << std::endl;

    print_arrays(fb, bh, n, k);
    
    calculate_error(fb, bh, n);

    free(fb);
    free(bh);
    delete nbody;
    // delete quadTree;
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

void print_points(Point* fb, Point* bh, int n) {
    for (int i = 0; i < n; ++i) {
        printf("fb[%i] = (%10.7lf, %10.7lf)\t", i, fb[i].x, fb[i].y);
        printf("bh[%i] = (%10.7lf, %10.7lf)\n", i, bh[i].x, bh[i].y);
    }
}

void print_arrays(Point* fb, Point* bh, int n, int k) {
    int m = n < 100 ? n : 100;
    std::cout << "fb_data = [";
    for (int i = 0; i < m; ++i) {
        if (i % 3 == 0) {
            std::cout << "\n\t";
        }
        printf("(%lf, %lf), ", fb[i].x, fb[i].y);
    }
    std::cout << "\n]" << std::endl;
    std::cout << "\nbh_data = [";
    for (int i = 0; i < m; ++i) {
        if (i % 3 == 0) {
            std::cout << "\n\t";
        }
        printf("(%lf, %lf), ", bh[i].x, bh[i].y);
    }
    std::cout << "\n]\n" << std::endl;
    std::cout << "n = " << n << std::endl;
    std::cout << "step = " << k << std::endl;
}

void calculate_error(Point *fb, Point *bh, int n) {
    //  !! calcular errores cada x pasos
    double total_error = 0.0;
    #pragma omp parallel for reduction(+:total_error)
    for (int i = 0; i < n; ++i) {
        double dx = fb[i].x - bh[i].x;
        double dy = fb[i].y - bh[i].y;
        double distance_error = sqrt(dx * dx + dy * dy);
        total_error += distance_error;
    }
    double mean_error = total_error / n;

    if (mean_error > EPSILON) {
        std::cout << "Error promedio (" << mean_error << "). Excede la tolerancia: (" << EPSILON << ")" << std::endl;
    } else if (mean_error < EPSILON) {
        std::cout << "Error promedio (" << mean_error << "). Tolerancia: (" << EPSILON << ")" << std::endl;
    }
}