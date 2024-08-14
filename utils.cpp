#include "utils.h"

// Function to calculate Z-order index for a 3D point
unsigned int calculateZOrderIndex(const Body &body) {
    unsigned int x = (unsigned int)(body.x * 1024);
    unsigned int y = (unsigned int)(body.y * 1024);

    x = (x | (x << 16)) & 0x030000FF;
    x = (x | (x << 8)) & 0x0300F00F;
    x = (x | (x << 4)) & 0x030C30C3;
    x = (x | (x << 2)) & 0x09249249;

    y = (y | (y << 16)) & 0x030000FF;
    y = (y | (y << 8)) & 0x0300F00F;
    y = (y | (y << 4)) & 0x030C30C3;
    y = (y | (y << 2)) & 0x09249249;

    return x | (y << 1);
}

// Comparator function to sort bodies based on Z-order index
bool compareZOrder(Body a, Body b) {
    unsigned int zOrderA = calculateZOrderIndex(a);
    unsigned int zOrderB = calculateZOrderIndex(b);
    return zOrderA < zOrderB;
}

// Function to sort bodies using Z-order index
void sortBodiesZOrder(Body *bodies, int n) {
    std::sort(bodies, bodies + n, compareZOrder);
}
// HILBERT

uint32_t interleave(uint32_t x){
    x = (x | (x << 16)) & 0x030000FF;
    x = (x | (x << 8)) & 0x0300F00F;
    x = (x | (x << 4)) & 0x030C30C3;
    x = (x | (x << 2)) & 0x09249249;
    return x;
}

// Function to calculate Hilbert index for a 3D point
uint32_t hilbertXYZToIndex(uint32_t x, uint32_t y){
    x = interleave(x);
    y = interleave(y);
    return x | (y << 1);
}

// Function to calculate Hilbert index for a 3D point
unsigned int calculateHilbertIndex(const Body &body) {
    // Normalize coordinates to be in range [0, 1023]
    unsigned int x = (unsigned int)((body.x + 1.0f) * 512.0f);
    unsigned int y = (unsigned int)((body.y + 1.0f) * 512.0f);

    return hilbertXYZToIndex(x, y);
}

// Comparator function to sort bodies based on Hilbert index
bool compareHilbertOrder(Body a, Body b) {
    unsigned int hilbertA = calculateHilbertIndex(a);
    unsigned int hilbertB = calculateHilbertIndex(b);
    return hilbertA < hilbertB;
}

// Function to sort bodies using Hilbert index
void sortBodiesHilbert(Body *bodies, int n) {
    std::sort(bodies, bodies + n, compareHilbertOrder);
}