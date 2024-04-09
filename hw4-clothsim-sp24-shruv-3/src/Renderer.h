// Renderer.h
#ifndef RENDERER_H
#define RENDERER_H

#include <GLFW/glfw3.h>
#include <cstdlib>

class Renderer {
public:
    static void renderParticles(int windowWidth, int windowHeight, int numberOfParticles) {
        glBegin(GL_POINTS);
        for (int i = 0; i < numberOfParticles; ++i) {
            // Generate random positions for the particles
            float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * windowWidth;
            float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * windowHeight;
            // Convert pixel coordinates to OpenGL coordinates (-1 to 1)
            float glX = (x / windowWidth) * 2.0f - 1.0f;
            float glY = (y / windowHeight) * 2.0f - 1.0f;
            glVertex2f(glX, glY);
        }
        glEnd();
    }
};

#endif // RENDERER_H
