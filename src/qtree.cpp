#include "qtree.h"

bool Quad::contains( const Body &body) const{
    float left = x - w / 2;
    float right = x + w / 2;
    float top = y + h / 2;
    float bottom = y - h / 2;
    return (body.x >= left && body.x <= right && body.y <= top && body.y >= bottom);
};

bool Quad::intersects(const Quad &range) const {
    return (
        x - w / 2 <= range.x + range.w / 2 &&
        x + w / 2 >= range.x - range.w / 2 &&
        y - h / 2 <= range.y + range.h / 2 &&
        y + h / 2 >= range.y - range.h / 2
    );
}

void QuadTree::clear() {
    for (int i = 0; i < size; ++i) {
        //bodies[i] = Body(); // Reiniciar el elemento usando el constructor predeterminado
        indices[i] = -1;    // Clear indices
    }
    size = 0; // Reiniciar el tamaño del array
}

void QuadTree::subdivide() {
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

// TODO: Refactorizar, parametros innecesarios: body = bodies[index]
// TODO: se pasa bodies ahora porque es arreglo global dentro del scope main
void QuadTree::insert(Body &body, int index, Body *bodies) {
    if (!boundary.contains(body)) return;

    if (size < capacity) {
        //bodies[size] = body;
        indices[size] = index; // acceder a arreglo global
        size++;
        updateCenterOfMass(body);
        return;
    }
    
    if (!isDivided) {
        subdivide();
    }

    for(int i = 0; i < size; ++i){
        int idx = indices[i];
        Body b = bodies[idx]; // TODO: arreglar esto
        if (topLeft->boundary.contains(b)) {
            topLeft->insert(b, idx, bodies);
        } else if (topRight->boundary.contains(b)) {
            topRight->insert(b, idx, bodies);
        } else if (bottomLeft->boundary.contains(b)) {
            bottomLeft->insert(b, idx, bodies);
        } else if (bottomRight->boundary.contains(b)) {
            bottomRight->insert(b, idx, bodies);
        }
    }

    clear();

    // Insertar el nuevo cuerpo
    if (topLeft->boundary.contains(body)) {
        topLeft->insert(body, index, bodies);
    } else if (topRight->boundary.contains(body)) {
        topRight->insert(body, index, bodies);
    } else if (bottomLeft->boundary.contains(body)) {
        bottomLeft->insert(body, index, bodies);
    } else if (bottomRight->boundary.contains(body)) {
        bottomRight->insert(body, index, bodies);
    }
    updateCenterOfMass(body);
}

// Centro de masa del cuadrante, pasar bodies como argumento
void QuadTree::updateCenterOfMass(const Body &body) {
    float totalMass = mass + body.mass;
    comX = (comX * mass + body.x * body.mass) / totalMass;
    comY = (comY * mass + body.y * body.mass) / totalMass;
    mass = totalMass;
}

// Fuerza que actua sobre un cuerpo, calculada con Barnes-Hut
// TODO: revisar parametros 
void QuadTree::computeForce(Body &body, float THETA, Body *bodies) const {
    /* if (size == 1 && &body != &bodies[0]) {
        body.addForce(bodies[0]);
    } */ 
    if (size <= 64) {
        // se recorre el arreglo de indices
        for(int i = 0; i < size; i++) {
            if(&body != &bodies[indices[i]]){
                body.addForce(bodies[indices[i]]);
            }
        }
    } else if (isDivided) {
        float s = boundary.w; // tamaño ancho
        // Distancia entre el cuerpo y el centro de masa del cuadrante
        float d = std::sqrt((comX - body.x) * (comX - body.x) + (comY - body.y) * (comY - body.y));
        // quitar el false dsps
        if (false && (s / d) < THETA) { // factor de barnes
            Body comBody(comX, comY, 0, 0, 0, 0, mass); // centro del cuadrante 
            body.addForce(comBody);
        } else {
            topLeft->computeForce(body, THETA, bodies);
            topRight->computeForce(body, THETA, bodies);
            bottomLeft->computeForce(body, THETA, bodies);
            bottomRight->computeForce(body, THETA, bodies);
        }
    }
}
