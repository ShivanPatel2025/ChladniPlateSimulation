#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Renderer.h"
#include "Particle.h"
#include <cmath>

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    const int windowWidth = 640;
    const int windowHeight = 480;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Chladni Plate Simulation", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Create particles
    std::vector<Particle> particles;
    for (int i = 0; i < 1000; ++i) {
        particles.emplace_back(rand() % windowWidth, rand() % windowHeight);
    }

    const float waveSpeed = 0.01f;
    const float waveFrequency = 0.02f;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Update particles based on wave pattern
        for (auto& particle : particles) {
            particle.x += sin(particle.y * waveFrequency + glfwGetTime() * waveSpeed) * 5.0f;
            // Ensure particles stay within window bounds
            if (particle.x >= windowWidth) particle.x -= windowWidth;
            if (particle.x < 0) particle.x += windowWidth;
        }

        // Render particles
        Renderer::renderParticles(particles, windowWidth, windowHeight);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
