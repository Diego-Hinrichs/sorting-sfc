#ifndef MORTON_H
#define MORTON_H
#include "body.hpp"

// TODO: Ahora deberian utilizar Point
unsigned int calculateZOrderIndex(const Body &body);
bool compareZOrder(Body a, Body b);
void sortBodiesZOrder(Body *bodies, int n);

#endif