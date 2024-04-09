// main.cpp
#include <GLFW/glfw3.h> // Make sure to link against GLFW or the library you choose for window management
#include "Simulation.h"
#include "Utils.h"
#include <iostream>

int main() {
    // Initialize GLFW, create a window, and make its context current

    // Main loop where you update the simulation and render
    while (!glfwWindowShouldClose(window)) {
        // Poll for and process events
        glfwPollEvents();

        // Render here

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
