#include "hilbert.hpp"

/* // HILBERT
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
} */

void AxestoTranspose2D(coord_t* X, int b) {
    coord_t M = 1 << (b - 1), P, Q, t;

    for (Q = M; Q > 1; Q >>= 1) {
        P = Q - 1;
        for (int i = 0; i < 2; i++) {
            if (X[i] & Q)
                X[0] ^= P;
            else {
                t = (X[0] ^ X[i]) & P;
                X[0] ^= t;
                X[i] ^= t;
            }
        }
    }

    for (int i = 1; i < 2; i++)
        X[i] ^= X[i - 1];
    t = 0;
    for (Q = M; Q > 1; Q >>= 1)
        if (X[1] & Q)
            t ^= Q - 1;
    for (int i = 0; i < 2; i++)
        X[i] ^= t;
}

bool compareHilbertOrder(const Body& a, const Body& b, int bits) {
    coord_t x1 = static_cast<coord_t>((a.x + 1.0f) * (1 << (bits - 1)));
    coord_t y1 = static_cast<coord_t>((a.y + 1.0f) * (1 << (bits - 1)));
    coord_t x2 = static_cast<coord_t>((b.x + 1.0f) * (1 << (bits - 1)));
    coord_t y2 = static_cast<coord_t>((b.y + 1.0f) * (1 << (bits - 1)));

    coord_t X1[] = {x1, y1};
    coord_t X2[] = {x2, y2};

    AxestoTranspose2D(X1, bits);
    AxestoTranspose2D(X2, bits);

    return std::lexicographical_compare(X1, X1 + 2, X2, X2 + 2);
}

void sortBodiesHilbert(Body* bodies, unsigned long n, int bits) {
    std::sort(bodies, bodies + n, [bits](const Body& a, const Body& b) {
        return compareHilbertOrder(a, b, bits);
    });
}
