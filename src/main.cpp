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
unsigned int seed = 22222;

// Barnes-Hut params
double THETA = 0.0;
int max_capacity;

// System
double G = 1.0;
double delta_time = 1e-4;
double softening_factor = 1e-10;
int n;

// window
Window* mainWindow;
const int WIDTH = 800;
const int HEIGHT = 800;
const char* title;

static const char *vShader = "src/shaders/point.vert";
static const char *fShader = "src/shaders/tree.frag";
std::vector<Shader> shaderList;

void calculate_boundary(Point* points, int n, double& min_x, double& max_x, double& min_y, double& max_y);
Quad dynamic_boundary(Point* points);

void CreateShaders() {
    Shader *shader = new Shader();
    shader->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader);
}

// !! DOS TIPOS DE SIMULACIONS
// !! SIN RADIO FIJO NBODY (BRUTO Y BH)
// !! CON RADIO FIJO SPH (BRUTO Y GRID) radix sort

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
    
    max_capacity = n;

    NBody* nbody = new NBody(n, G, delta_time, softening_factor);
    QuadTree* quadTree;
    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    // initialize_points(fb, n, seed);
    initialize_points_ring(fb, n,  seed, 0.5, 1.0);
    for (int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    if (alg == 1) { 
        title = "Direct Sum";
        error_check = false;
    } else if (alg == 2) {
        Quad boundary = Quad(0.0, 0.0, 1e6, 1e6);
        quadTree = new QuadTree(n, G, delta_time, softening_factor, max_capacity, boundary);
        title = "Barnes-Hut";
    } else if (alg > 2) {
        debug_mode(n, k, 100, max_capacity);
        return EXIT_SUCCESS;
    }

    // window and shaders
    mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->init(title);

    Mesh* mesh = new Mesh();
    Mesh* quadMesh = new Mesh();
    mesh->createMesh(dimension, n);
    quadMesh->createQuadMesh();

    CreateShaders();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // fondo
    shaderList[0].UseShader();
    GLuint size = shaderList[0].GetSizeLocation();
    glUniform1f(size, 2.0f);

    int step = 0;
    std::cout << "Simulacion iniciada: " << title << std::endl;
    printf("Steps: %i - Points: %i\n", k, n);
    while (!mainWindow->getShouldClose() && step < k) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (alg == 1) {
            nbody->simulate_fb(fb);
        } else if (alg == 2) { 
            glUniform4f(glGetUniformLocation(shaderList[0].GetColorLocation(), "color"), 0.0f, 1.0f, 0.0f, 1.0f);
            glLineWidth(1.0f);

            for (int i = 0; i < n; ++i) {
                quadTree->root->insert(bh, i);
            }

            // quadTree->root->boundary_ = dynamic_boundary(bh);
            // quadTree->root->boundary_ = Quad (0.0, 0.0, 1e6, 1e6);

            quadMesh->renderQuadTree(quadTree->root);

            // reset forces
            for (int i = 0; i < n; ++i) {
                bh[i].fx = 0.0;
                bh[i].fy = 0.0;
                bh[i].fz = 0.0;
            }

            for (int i = 0; i < n; ++i) {
                quadTree->root->calculate_force_node(bh, i, softening_factor, THETA, G);
            }
            
            update_position(bh, n, delta_time);
            
            quadTree->root->clear();

        }
        step++;

        Point* temp = alg == 1 ? fb : bh;
        glUniform4f(glGetUniformLocation(shaderList[0].GetColorLocation(), "color"), 0.9f, 0.9f, 0.9f, 1.0f);
        
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
        calculate_error(fb, bh, n, step);
    }
    
    delete[] fb;
    delete[] bh;
    delete nbody;
    return 0;
}

void calculate_boundary(Point* points, int n, double& min_x, double& max_x, double& min_y, double& max_y) {
    min_x = points[0].x;
    max_x = points[0].x;
    min_y = points[0].y;
    max_y = points[0].y;

    for (int i = 1; i < n; ++i) {
        if (points[i].x < min_x) min_x = points[i].x;
        if (points[i].x > max_x) max_x = points[i].x;
        if (points[i].y < min_y) min_y = points[i].y;
        if (points[i].y > max_y) max_y = points[i].y;
    }
}

Quad dynamic_boundary(Point* points){
    double min_x, max_x, min_y, max_y;
    calculate_boundary(points, n, min_x, max_x, min_y, max_y);
    double width = max_x - min_x;
    double height = max_y - min_y;
    double centerX = (min_x + max_x) / 2.0;
    double centerY = (min_y + max_y) / 2.0;
    double boundary_size = std::max(width, height);
    double margin = 0.01 * boundary_size;
    Quad boundary = Quad(centerX, centerY, boundary_size + margin, boundary_size + margin);
    // Quad boundary = Quad(0.0, 0.0, boundary_size + margin, boundary_size + margin);
    return boundary;
}

bool debug_mode(int n, int k, int x, int max_capacity) {
    Point* fb = (Point*)malloc(n * sizeof(Point));
    Point* bh = (Point*)malloc(n * sizeof(Point));

    // initialize_points(fb, n, seed);
    initialize_points_ring(fb, n,  seed, 0.5, 1.0);
    for (int i = 0; i < n; ++i) {
        bh[i] = fb[i];
    }

    NBody* nbody = new NBody(n, G, delta_time, softening_factor);
    QuadTree* quadTree = new QuadTree(n, G, delta_time, softening_factor, max_capacity, dynamic_boundary(bh));

    int step = 0;
    double max_error_percent = 0.0;
    while (step < k) {
        nbody->simulate_fb(fb);
        
        quadTree->root->boundary_ = dynamic_boundary(bh);
        quadTree->insert(bh);
        quadTree->simulate_bh(bh, THETA);
        // calculate_error(fb, bh, n, step);
        double fb_total_energy = total_energy(fb, n);
        double bh_total_energy = total_energy(bh, n);
        
        double error_energy = std::abs(fb_total_energy - bh_total_energy);
        double error_percent = std::abs(error_energy / fb_total_energy) * 100.0;
        if (error_percent > max_error_percent) {
            max_error_percent = error_percent;
        }

        std::cout << step << ";";
        std::cout << fb_total_energy << ";";
        std::cout << bh_total_energy << ";";
        std::cout << error_energy << ";";
        std::cout << error_percent <<  "%;";
        std::cout << max_error_percent <<  "%" << std::endl;

        step++;
    }

    free(fb);
    free(bh);
    delete nbody;
    return true;
}
