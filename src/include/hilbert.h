#ifndef HILBERT_H
#define HILBERT_H
#include "body.h"

void AxestoTranspose2D(coord_t* X, int b);
bool compareHilbertOrder(const Body& a, const Body& b, int bits);
void sortBodiesHilbert(Body* bodies, unsigned long n, int bits);

#endif