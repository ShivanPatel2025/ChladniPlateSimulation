#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

// Define the Particle structure
struct Particle {
    float x, y;     // Position
    float vx, vy;   // Velocity
    Particle(float x, float y) : x(x), y(y), vx(0), vy(0) {}
};

// Function prototypes
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void initializeParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight);
void updateParticles(std::vector<Particle>& particles, float waveSpeed, float waveFrequency, int windowWidth, int windowHeight, bool isPlaying);

// Global variables for simulation state
bool isPlaying = false;
bool needsReset = false;

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
    glfwSetKeyCallback(window, keyCallback); // Set the key callback for GLFW

    // Create and initialize particles
    std::vector<Particle> particles;
    initializeParticles(particles, windowWidth, windowHeight);

    const float waveSpeed = 0.01f;
    const float waveFrequency = 0.02f;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Reset the simulation if needed
        if (needsReset) {
            initializeParticles(particles, windowWidth, windowHeight);
            needsReset = false;
        }
        glPointSize(3.0f);
        // Update particles based on wave pattern if the simulation is playing
        updateParticles(particles, waveSpeed, waveFrequency, windowWidth, windowHeight, isPlaying);

        // Render particles
        glBegin(GL_POINTS);
        for (const auto& particle : particles) {
            // Convert pixel coordinates to OpenGL coordinates (-1 to 1)
            float glX = (particle.x / windowWidth) * 2.0f - 1.0f;
            float glY = (particle.y / windowHeight) * 2.0f - 1.0f;
            glVertex2f(glX, glY);
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_SPACE: // Toggle play/pause
                isPlaying = !isPlaying;
                break;
            case GLFW_KEY_R: // Reset
                needsReset = true;
                break;
        }
    }
}

void initializeParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight) {
    particles.clear();
    for (int i = 0; i < 10000; ++i) { // Adjust particle count as desired
        particles.emplace_back(rand() % windowWidth, rand() % windowHeight);
    }
}

void updateParticles(std::vector<Particle>& particles, float waveSpeed, float waveFrequency, int windowWidth, int windowHeight, bool isPlaying) {
    if (!isPlaying) return;

    for (auto& particle : particles) {
        // Simple wave influence on particle movement
        particle.x += sin(particle.y * waveFrequency + glfwGetTime() * waveSpeed) * 5.0f;

        // Keep particles within window bounds
        if (particle.x >= windowWidth) particle.x -= windowWidth;
        if (particle.x < 0) particle.x += windowWidth;
    }
}
