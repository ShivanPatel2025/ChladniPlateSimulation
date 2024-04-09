// Renderer.h
#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "Particle.h"
#include <GLFW/glfw3.h>

class Renderer {
public:
    static void renderParticles(const std::vector<Particle>& particles, int windowWidth, int windowHeight) {
        glBegin(GL_POINTS);
        for (const auto& particle : particles) {
            // Convert pixel coordinates to OpenGL coordinates (-1 to 1)
            float glX = (particle.x / windowWidth) * 2.0f - 1.0f;
            float glY = (particle.y / windowHeight) * 2.0f - 1.0f;
            glVertex2f(glX, glY);
        }
        glEnd();
    }
};

#endif // RENDERER_H
