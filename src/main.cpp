#include <cstdlib>
#include <omp.h>
#include <chrono>
#include <math.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include "window.hpp"
#include "shader.hpp"
#include "mesh.hpp"

#include "utils.hpp"

#include "qtree.hpp"
#include "nbody.hpp"

bool error_check = true;

int dimension = 2;
unsigned int seed = 12345;

// Barnes-Hut params
double THETA = 0.0;
int max_capacity;

// System
double G = 1.0;
double delta_time = 1e-4;
double softening_factor = 1e-12;
double size = 10.0;
int n;

// window
Window* mainWindow;
const int WIDTH = 800;
const int HEIGHT = 800;
const char* title;

static const char *fShader = "src/shaders/point.frag";
static const char *vShader = "src/shaders/point.vert";
std::vector<Shader> shaderList;
void CreateShaders() {
    Shader *shader = new Shader();
    shader->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader);
}

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
    
    max_capacity = 1;

    QuadTree* quadTree;
    Quad rootBoundaryfb(0.0, 0.0, size, size);
    Quad rootBoundarybh(0.0, 0.0, size, size);
    NBody* nbody = new NBody(n, G, delta_time, softening_factor, rootBoundaryfb);

    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    initialize_points(fb, n, seed);
    for (int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    if (alg == 1) { 
        title = "Direct Sum";
        error_check = false;
    } else if (alg == 2) { 
        quadTree = new QuadTree(n, G, delta_time, softening_factor, max_capacity, rootBoundarybh);
        title = "Barnes-Hut";
    } else if (alg > 2) {
        debug_mode(n, k, 100, max_capacity);
        return EXIT_SUCCESS;
    }

    // window and shaders
    mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->init(title);
    Mesh *mesh = new Mesh();
    mesh->createMesh(dimension, n);
    CreateShaders();
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    shaderList[0].UseShader();
    GLuint size = shaderList[0].GetSizeLocation();
    glUniform1f(size, 4.0f);

    int step = 0;
    std::cout << "\nSimulacion iniciada: " << title << std::endl;
    printf("Steps: %i - Points: %i\n", k, n);
    while (!mainWindow->getShouldClose() && step < k) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (alg == 1) {
            nbody->simulate_fb(fb);
        } else if (alg == 2) { 
            quadTree->insert(bh);               
            quadTree->simulate_bh(bh, THETA);
        }
        step++;

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

bool debug_mode(int n, int k, int x, int max_capacity) {
    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    initialize_points(fb, n, seed);
    for (int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    std::cout << "\nSimulacion iniciada." << std::endl;
    printf("Steps: %i - Points: %i\n", k, n);
    printf("G: %.1lf - dt: %.1e - Soft Factor: %.1e - THETA: %.1lf - Capacity x node: %i point(s)\n", G, delta_time, softening_factor, THETA, max_capacity);
    // print_points(fb, bh, n < 10 ? n : 10);

    Quad rootBoundaryfb(0.0, 0.0, size, size);
    Quad rootBoundarybh(0.0, 0.0, size, size);

    NBody* nbody = new NBody(n, G, delta_time, softening_factor, rootBoundaryfb);
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

