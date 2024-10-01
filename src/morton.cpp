#include "morton.h"

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
