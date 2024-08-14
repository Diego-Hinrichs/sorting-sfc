#include <random>
#include <omp.h>
#include <iostream>
#include <string.h>
#include "utils.h"

// N-Body Barnes-Hut
const float THETA = 0.5; // Apertura

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
                Body comBody(comX, comY, 0, 0, 0, 0, mass);
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
Body *bodies;

void simulateBH(float dt, int numBodies){
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

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <n> <alg [0|1]> <order [0|1|2]> <visualize [0|1]> <seed>\n", argv[0]);
        fprintf(stderr, "\nn: num bodies\nalg: [0]Brute Force or [1]Barnes-Hut\norder: [0]none, [1]Morton or [2]Hilbert\nvisualize: [0]no or [1]yes\nseed: seed for random generator");
        return 1;
    }

    int numBodies = atoi(argv[1]);
    unsigned int alg = atoi(argv[2]);
    unsigned int order = atoi(argv[3]);
    unsigned int visualize = atoi(argv[4]);
    unsigned int seed = atoi(argv[5]);
    
    bodies = (Body*)malloc(numBodies * sizeof(Body));
    initBodies(bodies, numBodies, seed);

    // TODO: Proximo paso
    // TODO: Guard en un arreglo p[n], con los indices de los puntos
    // TODO: así el sorting se realizara sobre el arreglo y no sobre los puntos como tal
    // TODO: luego construir el barnes hut con indices


    // bins con los indices de los puntos maximo k elementos x bins
    // [ ,   , ]
    // [ , 3 , ]
    // [ ,   , ]

    if(alg == 1){
        Quad boundary(0, 0, 3500, 3500);
        quadTree = new QuadTree(boundary, 64);

        for (int i = 0; i < numBodies; ++i) {
            quadTree->insert(bodies[i]);
        }

        int step = 0;
        while (step < 3000) {
            simulateBH(0.016, numBodies);
            delete quadTree;
            quadTree = new QuadTree(boundary, 64);

            for (int i = 0; i < numBodies; ++i) {
                quadTree->insert(bodies[i]);
            }
            step++;
        }
        delete quadTree;
    } else if (alg == 0){
        int step = 0;
        while (step < 3000) {
            updateForce(bodies, numBodies, 1.0);
            updatePosition(bodies, numBodies, 0.016);
            step++;
        }
    };
    free(bodies);
    return 0;
}
