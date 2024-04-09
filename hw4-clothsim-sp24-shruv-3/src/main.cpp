#include <GLFW/glfw3.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Renderer.h"
#include "Plate.h"
#include "Simulation.h"

#define GL_SILENCE_DEPRECATION
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render sand particles
        Renderer::renderParticles(windowWidth, windowHeight, 1000); // Feel free to adjust the number of particles

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
