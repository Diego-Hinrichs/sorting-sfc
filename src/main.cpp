#include <iostream>
#include <ctime>
#include <chrono>
#include <string.h>
#include "morton.h"
#include "hilbert.h"
#include "qtree.h"

using namespace std::chrono;

// N-Body Barnes-Hut
const float THETA = 0.0; // Apertura
Body *bodies;

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

// ver como recorrer el arbol y los vecinos
void simulateBH(float dt, int numBodies){
    for(int i = 0; i < numBodies; ++i){
        bodies[i].resetForce();
    }
    // Esta ordenando en este paso
    for(int i = 0; i < numBodies; ++i){
        quadTree->computeForce(bodies[i], THETA, bodies);
    }
    
    for(int i = 0; i < numBodies; ++i){
        bodies[i].update(dt);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <n> <alg [0|1]> <order [0|1|2]> <seed>\n", argv[0]);
        return 1;
    }

    int numBodies = atoi(argv[1]);
    unsigned int alg = atoi(argv[2]);
    unsigned int order = atoi(argv[3]);
    unsigned int seed = atoi(argv[4]);
    
    bodies = (Body*)malloc(numBodies * sizeof(Body));
    initBodies(bodies, numBodies, seed);
    // Ordenar los cuerpos y los índices según Morton o Hilbert
    if (order == 1) {
        sortBodiesZOrder(bodies, numBodies);
    } else if (order == 2) {
        sortBodiesHilbert(bodies, numBodies, 32);
    }

    // si es barnes-hut
    if (alg == 1) {
        Quad boundary(0, 0, 3500, 3500);
        quadTree = new QuadTree(boundary, 64);

        for (int i = 0; i < numBodies; ++i) {
            quadTree->insert(bodies[i], i, bodies);
        }

        int step = 0;
        while (step < 3000) {
            simulateBH(0.016, numBodies);
            delete quadTree;
            quadTree = new QuadTree(boundary, 64);

            for (int i = 0; i < numBodies; ++i) {
                quadTree->insert(bodies[i], i, bodies);
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
    
    free(bodies);
    return 0;
    }
}
