#ifndef RENDERER_H
#define RENDERER_H

#include "Simulation.h"

class Renderer {
public:
    Renderer();
    void render(const Simulation& simulation);
    // Additional methods for setting up and managing render states
};

#endif // RENDERER_H
