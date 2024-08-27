#include <fstream>
#include <random>
#include "hilbert.h"

// Función auxiliar que calcula la clave Peano-Hilbert
peanokey peano_hilbert_key(int x, int y, int bits) {
    int i, quad, bitx, bity;
    int mask, rotation, rotx, roty, sense;
    peanokey key;

    static const int quadrants[24][2][2] = {
        {{0, 7}, {1, 6}}, {{3, 4}, {2, 5}},
        {{4, 3}, {5, 2}}, {{7, 0}, {6, 1}},
        {{1, 0}, {6, 7}}, {{2, 3}, {5, 4}},
        {{3, 2}, {4, 5}}, {{0, 1}, {7, 6}},
        {{6, 1}, {7, 0}}, {{5, 2}, {4, 3}},
        {{2, 5}, {3, 4}}, {{1, 6}, {0, 7}},
        {{7, 6}, {0, 1}}, {{4, 5}, {3, 2}},
        {{5, 4}, {2, 3}}, {{6, 7}, {1, 0}},
    };

    static const int rotxmap_table[24] = {
        4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 0, 1, 2, 3,
    };

    static const int rotymap_table[24] = {
        1, 2, 3, 0, 16, 17, 18, 19,
        11, 8, 9, 10, 22, 23, 20, 21,
    };

    static const int rotx_table[8] = { 3, 0, 0, 2, 2, 0, 0, 1 };
    static const int roty_table[8] = { 0, 1, 1, 2, 2, 3, 3, 0 };
    static const int sense_table[8] = { -1, -1, -1, +1, +1, -1, -1, -1 };

    mask = 1 << (bits - 1);
    key = 0;
    rotation = 0;
    sense = 1;

    for (i = 0; i < bits; i++, mask >>= 1) {
        bitx = (x & mask) ? 1 : 0;
        bity = (y & mask) ? 1 : 0;

        quad = quadrants[rotation][bitx][bity];

        key <<= 2;
        key += (sense == 1) ? (quad) : (3 - quad);

        rotx = rotx_table[quad];
        roty = roty_table[quad];
        sense *= sense_table[quad];

        while (rotx > 0) {
            rotation = rotxmap_table[rotation];
            rotx--;
        }

        while (roty > 0) {
            rotation = rotymap_table[rotation];
            roty--;
        }
    }

    return key;
}

// Comparador para ordenar cuerpos según la clave Peano-Hilbert
bool compareHilbertOrder(const Body& a, const Body& b) {
    int bits = 10;
    // Normalización de coordenadas de [-1, 1] a [0, 1]
    float normalized_x1 = (a.x + 1.0f) * 0.5f;
    float normalized_y1 = (a.y + 1.0f) * 0.5f;
    float normalized_x2 = (b.x + 1.0f) * 0.5f;
    float normalized_y2 = (b.y + 1.0f) * 0.5f;

    // Escalar a enteros
    int x1 = static_cast<int>(normalized_x1 * (1 << bits));
    int y1 = static_cast<int>(normalized_y1 * (1 << bits));
    int x2 = static_cast<int>(normalized_x2 * (1 << bits));
    int y2 = static_cast<int>(normalized_y2 * (1 << bits));
    peanokey key1 = peano_hilbert_key(x1, y1, bits);
    peanokey key2 = peano_hilbert_key(x2, y2, bits);

    return key1 < key2;
}

// Función para ordenar cuerpos usando la curva Peano-Hilbert
void sortBodiesHilbert(Body* bodies, unsigned long n) {
    std::sort(bodies, bodies + n, compareHilbertOrder);
}

// Función para inicializar cuerpos
void initBodies(Body* bodies, int numBodies, unsigned int seed) {
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<float> distribution(-10.0, 10.0);
    std::uniform_real_distribution<float> massDistribution(1.0, 10.0);

    for (int i = 0; i < numBodies; i++) {
        bodies[i].x = distribution(generator);
        bodies[i].y = distribution(generator);
    }
}

// Función para guardar cuerpos en un archivo .txt
void saveBodiesToFile(const Body* bodies, int numBodies, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < numBodies; ++i) {
            file << bodies[i].x << " " << bodies[i].y << "\n";
        }
        file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo " << filename << " para escribir.\n";
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Uso: " << argv[0] << " n\n";
        return 1;
    }
    const int numBodies = atoi(argv[1]);
    Body bodies[numBodies];

    // Inicializar cuerpos
    initBodies(bodies, numBodies, 42);

    // Guardar cuerpos antes del ordenamiento
    saveBodiesToFile(bodies, numBodies, "bodies_before_sorting.txt");

    // Ordenar cuerpos usando Peano-Hilbert
    sortBodiesHilbert(bodies, numBodies);

    // Guardar cuerpos después del ordenamiento
    saveBodiesToFile(bodies, numBodies, "bodies_after_sorting.txt");

    return 0;
}
