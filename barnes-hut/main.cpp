#include <iostream>
#include <random>
#include <ctime>
#include <cstring>
#include "utils.h"

const float THETA = 0.5;     // Parámetro de apertura para Barnes-Hut

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
};

struct QuadTree {
    int capacity;
    int size = 0;
    
    float mass = 0; // Total mass in this QuadTree node
    float comX = 0, comY = 0; // Center of mass
    
    bool isDivided = false;

    Quad boundary;
    Body *bodies = (Body*)malloc(capacity * sizeof(Body));

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
        free(bodies);
    }

    void clear() {
        for (int i = 0; i < size; ++i) {
            bodies[i] = Body(); // Reiniciar el elemento usando el constructor predeterminado
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

    void insert(Body &body) {
        if (!boundary.contains(body)) return;

        if (size < capacity) {
            bodies[size] = body;
            size++;
            updateCenterOfMass(body);
            return;
        }
        
        if (!isDivided) {
            subdivide();
        }

        for(int i = 0; i < size; ++i){
            Body b = bodies[i];
            if (topLeft->boundary.contains(b)) {
                topLeft->insert(b);
            } else if (topRight->boundary.contains(b)) {
                topRight->insert(b);
            } else if (bottomLeft->boundary.contains(b)) {
                bottomLeft->insert(b);
            } else if (bottomRight->boundary.contains(b)) {
                bottomRight->insert(b);
            }
        }

        clear();

        // Insertar el nuevo cuerpo
        if (topLeft->boundary.contains(body)) {
            topLeft->insert(body);
        } else if (topRight->boundary.contains(body)) {
            topRight->insert(body);
        } else if (bottomLeft->boundary.contains(body)) {
            bottomLeft->insert(body);
        } else if (bottomRight->boundary.contains(body)) {
            bottomRight->insert(body);
        }
        updateCenterOfMass(body);
    }

    // Centro de masa del cuadrante
    void updateCenterOfMass(const Body &body) {
        float totalMass = mass + body.mass;
        comX = (comX * mass + body.x * body.mass) / totalMass;
        comY = (comY * mass + body.y * body.mass) / totalMass;
        mass = totalMass;
    }

    // Fuerza que actua sobre un cuerpo, calculada con Barnes-Hut
    void computeForce(Body &body) const {
        // Checkear si el cuerpo está en el cuadrante y no es el único
        // ! Este if ver si se puede quitar, es redundante
        if (size == 1 && &body != &bodies[0]) {
            body.addForce(bodies[0]);
        } else if (isDivided) {
            float s = boundary.w; // tamaño ancho
            // Distancia entre el cuerpo y el centro de masa del cuadrante
            float d = std::sqrt((comX - body.x) * (comX - body.x) + (comY - body.y) * (comY - body.y));
            if ((s / d) < THETA) {
                Body comBody(comX, comY, 0, 0, mass);
                body.addForce(comBody);
            } else {
                topLeft->computeForce(body);
                topRight->computeForce(body);
                bottomLeft->computeForce(body);
                bottomRight->computeForce(body);
            }
        }
    }
};

QuadTree *quadTree;
Body *bodies;

void simulate(float dt, int numBodies) {
    for(int i = 0; i < numBodies; ++i){
        bodies[i].resetForce();
    }
    
    for(int i = 0; i < numBodies; ++i){
        quadTree->computeForce(bodies[i]);
    }
    
    for(int i = 0; i < numBodies; ++i){
        bodies[i].update(dt);
    }
}

void initBodies(Body *bodies, int numBodies, unsigned int seed) {
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<float> distribution(-10.0, 10.0);
    std::uniform_real_distribution<float> massDistribution(1.0, 10.0);

    for(int i = 0; i < numBodies; i++){
        bodies[i].x = distribution(generator);
        bodies[i].y = distribution(generator);
        bodies[i].vx = distribution(generator);
        bodies[i].vy = distribution(generator);
        bodies[i].mass = massDistribution(generator);
    }
};

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <n: num bodies> <k: max capacity> <o: orderMode [1:zorder |2: hilbert]> <s: seed>\n", argv[0]);
        return 1;
    }

    int numBodies = atoi(argv[1]);
    int maxCapacity = atoi(argv[2]);
    unsigned int orderMode = atoi(argv[3]);
    unsigned int seed = atoi(argv[4]);

    std::cout << "Num Bodies: " << numBodies << std::endl;
    std::cout << "Max Capacity: " << maxCapacity << std::endl;

    Quad boundary(0, 0, 3500, 3500);
    quadTree = new QuadTree(boundary, maxCapacity);

    bodies = (Body*)malloc(numBodies * sizeof(Body));
    initBodies(bodies, numBodies, seed);

    for (int i = 0; i < numBodies; ++i) {
        quadTree->insert(bodies[i]);
    }

    // Opción 1, ordenar antes de constuir el QuadTree
    // TODO: GUARDAR INDICE EN EL ARBOL

    // TODO: ACCEDER AL CUERPO EN EL ARBOL

    // glutInit(&argc, argv);
    int step = 0;
    while (step < 3000) {
        simulate(0.016, numBodies);
        delete quadTree;
        quadTree = new QuadTree(boundary, maxCapacity);

        for (int i = 0; i < numBodies; ++i) {
            quadTree->insert(bodies[i]);
        }
        step++;
    }

    std::cout << "Termine! " << step << std::endl;
    delete quadTree;
    free(bodies);
    return 0;
}
