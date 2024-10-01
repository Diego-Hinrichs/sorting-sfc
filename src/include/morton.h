#ifndef MORTON_H
#define MORTON_H
#include "body.h"

unsigned int calculateZOrderIndex(const Body &body);
bool compareZOrder(Body a, Body b);
void sortBodiesZOrder(Body *bodies, int n);

#endif