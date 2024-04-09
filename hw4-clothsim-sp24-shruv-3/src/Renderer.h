#ifndef RENDERER_H
#define RENDERER_H

#include <GLFW/glfw3.h>
#include <cstdlib>

class Renderer {
public:
    static void renderParticles() {
        glBegin(GL_POINTS);
        for (int i = 0; i < 100; ++i) {
            glVertex2f(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                       static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }
        glEnd();
    }
};

#endif // RENDERER_H
