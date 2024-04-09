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

    GLFWwindow* window = glfwCreateWindow(640, 480, "Simple Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
