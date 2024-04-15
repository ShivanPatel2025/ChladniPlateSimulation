#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>



const float L1 = 0.04;
const float L2 = 0.02;
const float L3 = 0.018;
// Global index for the current chladni parameters
int currentParamIndex = 0;



const float PI = 3.14159265358979323846;

struct ChladniParams {
    int m, n;
    float l;
    ChladniParams(int m, int n, float l) : m(m), n(n), l(l) {}
};

std::vector<ChladniParams> chladniParams = {
        {1, 2, L1}, {1, 3, L3}, {1, 4, L2}, {1, 5, L2},
        {2, 3, L2}, {2, 5, L2}, {3, 4, L2}, {3, 5, L2}, {3, 7, L2}
};

struct Gradient {
    float dx, dy;
};


class Simulation {
public:
    std::vector<float> vibrationValues;
    std::vector<Gradient> gradients;
    int width, height;

    void computeVibrationValues(const ChladniParams& params) {
        vibrationValues.resize(width * height);
        float TX = std::rand() % height;  // Translate X
        float TY = std::rand() % height;  // Translate Y

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float scaledX = x * params.l + TX;
                float scaledY = y * params.l + TY;
                float NX = params.n * scaledX;
                float MX = params.m * scaledX;
                float NY = params.n * scaledY;
                float MY = params.m * scaledY;

                float value = std::cos(NX) * std::cos(MY) - std::cos(MX) * std::cos(NY);
                value /= 2;  // Normalize from [-2..2] to [-1..1]
                value *= std::copysign(1.0, value);  // Flip troughs to become crests

                vibrationValues[y * width + x] = value;
            }
        }
    }

    void computeGradients() {
        gradients.resize(width * height);
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 1; x < width - 1; ++x) {
                int index = y * width + x;
                float currentVibration = vibrationValues[index];
                if (std::abs(currentVibration) < 0.01) {  // Node threshold
                    gradients[index] = {0, 0};
                    continue;
                }

                Gradient bestGradient = {0, 0};
                float minVibration = std::numeric_limits<float>::max();

                for (int ny = -1; ny <= 1; ++ny) {
                    for (int nx = -1; nx <= 1; ++nx) {
                        if (nx == 0 && ny == 0) continue;

                        int neighborIndex = (y + ny) * width + (x + nx);
                        float neighborVibration = vibrationValues[neighborIndex];

                        if (neighborVibration < minVibration) {
                            minVibration = neighborVibration;
                            bestGradient = {float(nx), float(ny)};
                        }
                    }
                }

                gradients[index] = bestGradient;
            }
        }
    }
};


// Define the Particle structure
struct Particle {
    float x, y;     // Position
    Particle(float x, float y) : x(x), y(y) {}
};

// Function prototypes
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void initializeParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight);
void updateParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight, bool isRunning);
void renderParticles(const std::vector<Particle>& particles, int windowWidth, int windowHeight);

// Global variables for simulation state
bool isRunning = false;
bool needsResize = false;



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_SPACE: // Toggle running state
                isRunning = !isRunning;
                break;
            case GLFW_KEY_R: // Resize
                needsResize = true;
                break;
            case GLFW_KEY_UP: // Increase frequency pattern
                currentParamIndex = (currentParamIndex + 1) % chladniParams.size();
                needsResize = true;  // Force recalculation of patterns
                break;
            case GLFW_KEY_DOWN: // Decrease frequency pattern
                currentParamIndex = (currentParamIndex - 1 + chladniParams.size()) % chladniParams.size();
                needsResize = true;  // Force recalculation of patterns
                break;
        }
    }
}


void initializeParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    particles.clear();
    for (int i = 0; i < 30000; ++i) {
        particles.emplace_back(dis(gen) * windowWidth, dis(gen) * windowHeight);
    }
}

void updateParticles(std::vector<Particle>& particles, Simulation& sim, int windowWidth, int windowHeight, bool isRunning) {
    if (!isRunning) return;

    float slowFactor = 0.1; // Reduce speed by using a factor < 1

    for (auto& particle : particles) {
        if (particle.x < 0 || particle.x >= windowWidth || particle.y < 0 || particle.y >= windowHeight) {
            continue; // Skip update if particle is out of bounds
        }

        int index = static_cast<int>(particle.y) * windowWidth + static_cast<int>(particle.x);
        if (index < 0 || index >= sim.gradients.size()) {
            continue; // Safety check for valid index
        }

        Gradient grad = sim.gradients[index];

        // Apply a factor to slow down movement
        particle.x += grad.dx * slowFactor;
        particle.y += grad.dy * slowFactor;

        // Boundary checks (wrap around)
        if (particle.x >= windowWidth) particle.x = 0;
        if (particle.x < 0) particle.x = windowWidth - 1;
        if (particle.y >= windowHeight) particle.y = 0;
        if (particle.y < 0) particle.y = windowHeight - 1;
    }
}


void renderParticles(const std::vector<Particle>& particles, int windowWidth, int windowHeight) {
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (const auto& particle : particles) {
        float glX = (particle.x / windowWidth) * 2.0f - 1.0f;
        float glY = (particle.y / windowHeight) * 2.0f - 1.0f;
        glVertex2f(glX, glY);
    }
    glEnd();
}


int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    int windowWidth = 640;
    int windowHeight = 480;
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

    // Initialize Simulation
    Simulation sim;
    sim.width = windowWidth;
    sim.height = windowHeight;
    sim.computeVibrationValues(chladniParams[0]); // Initial computation with the first parameter set
    sim.computeGradients();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Check if parameters need updating
        if (needsResize) {
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glViewport(0, 0, windowWidth, windowHeight);
            initializeParticles(particles, windowWidth, windowHeight);
            sim.width = windowWidth;
            sim.height = windowHeight;
            sim.computeVibrationValues(chladniParams[currentParamIndex]); // Compute with new parameters
            sim.computeGradients();
            needsResize = false;
        }

        // Update particles if the simulation is running
        if (isRunning) {
            updateParticles(particles, sim, windowWidth, windowHeight, isRunning);
        }

        // Render particles
        renderParticles(particles, windowWidth, windowHeight);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
