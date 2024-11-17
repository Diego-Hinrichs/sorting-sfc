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
bool error_check;

int dimension = 2;
unsigned int seed = 12345;

// Barnes-Hut params
double THETA = 0.0;

// System
double G = 1.0;
double delta_time = 1e-4;
double softening_factor = 1e-12;
double size = 20.0;
int n;
int max_capacity;

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
    max_capacity = 1; // only one quadrant

    QuadTree* quadTree;
    Quad rootBoundarybh(0.0, 0.0, size, size);
    NBody* nbody = new NBody(n, G, delta_time, softening_factor);

    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));
    initialize_points(fb, n, seed);
    for (int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    if (alg == 1) { 
        title = "Brute Force Simulation";
        error_check = false;
    } else if (alg == 2) { 
        quadTree = new QuadTree(n, G, delta_time, softening_factor, max_capacity, rootBoundarybh);
        title = "Barnes-Hut Simulation";
        error_check = true;
    } else if (alg > 2) {
        debug_mode(n, k, 100);
        return EXIT_SUCCESS;
    }

    // Inicializar ventana, VAO y VBO
    mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->init(title);
    Mesh *mesh = new Mesh();
    mesh->createMesh(dimension, n);

    int step = 0;
    while (!mainWindow->getShouldClose() && step < k) {
        if (alg == 1) {
            nbody->simulate_fb(fb);
        } else if (alg == 2) { 
            quadTree->insert(bh);               
            quadTree->simulate_bh(bh, THETA);
        }
        step++;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Point* temp = alg == 1 ? fb : bh;
        mesh->renderMesh(temp, dimension, n);
        mesh->drawMesh(n);
        mainWindow->swapBuffers();
        glfwPollEvents();
    }
    mesh->~Mesh();
    mainWindow->~Window();

    std::cout << "Simulacion terminada en el paso: " << step << std::endl;
    if (error_check) {
        std::cout << "Simulando fuerza bruta.\n";
        for (int i = 0; i < step; ++i) {
            nbody->simulate_fb(fb);
        }
        std::cout << "Calculando error.\n";
        calculate_error(fb, bh, n);
    }
    
    delete[] fb;
    delete[] bh;
    delete nbody;
    delete quadTree;
    return 0;
}

bool debug_mode(int n, int k, int x) {
    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    initialize_points(fb, n, seed);
    for (int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    std::cout << "\nSimulacion iniciada." << std::endl;
    printf("Step: %i. Points: %i.\n", 0, n);
    // print_points(fb, bh, n < 10 ? n : 10);

    Quad rootBoundarybh(0.0, 0.0, size, size);
    max_capacity = 1; // only one quadrant

    NBody* nbody = new NBody(n, G, delta_time, softening_factor);
    QuadTree* quadTree = new QuadTree(n, G, delta_time, softening_factor, max_capacity, rootBoundarybh);
    
    // Fuerza Bruta
    for (int step = 0; step < k; ++step) {
        nbody->simulate_fb(fb);
    }

    // Barnes-Hut
    for (int step = 0; step < k; ++step) {
        quadTree->insert(bh);
        quadTree->simulate_bh(bh, THETA);
    }

    std::cout << "\nSimulacion terminada." << std::endl;
    printf("Step: %i. Points: %i.\n", k-1, n);
    // print_points(fb, bh, n < 10 ? n : 10);

    // print_arrays(fb, bh, n, k);
    std::cout << "\nCalculando error." << std::endl;
    calculate_error(fb, bh, n);

    free(fb);
    free(bh);
    delete nbody;
    delete quadTree;
    return true;
}

// TODO: Distribucion uniforme `acotada`
void initialize_points(Point* points, int n, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        double y = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
        points[i] = Point(x, y, 0.0, 1.0);
    }
}

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
