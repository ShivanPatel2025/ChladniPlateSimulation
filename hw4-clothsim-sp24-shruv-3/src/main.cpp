#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>


// Constants for different Chladni plate parameters.
const float L1 = 0.04;
const float L2 = 0.02;
const float L3 = 0.018;

// Index to track which Chladni parameter set is currently active.
int currentParamIndex = 0;

// Constant for PI.
const float PI = 3.141592653589793238462643383279502884197;

// Structure to store Chladni parameters including mode numbers (m, n) and scaling factor (l).
struct ChladniParams {
    int m, n;
    float l;
    ChladniParams(int m, int n, float l) : m(m), n(n), l(l) {}
};

// List of Chladni parameters configurations.
std::vector<ChladniParams> chladniParams = {
        {1, 2, L1}, {1, 3, L3}, {2, 3, L2}, {1, 4, L2}, {2, 4, L2}, {3, 4, L2}, {1, 5, L2},
          {2, 5, L2}, {3, 5, L2}, {3, 7, L2}
};

// Structure to store gradient vectors.
struct Gradient {
    float dx, dy;
};

// Class to manage the Chladni plate simulation.
class Simulation {
public:
    std::vector<float> vibrationValues; // Stores vibration values at each grid point.
    std::vector<Gradient> gradients;    // Stores gradient vectors for particle movement.
    int width, height;                  // Dimensions of the simulation grid.

     // Computes vibration values based on current Chladni parameters.
    void computeVibrationValues(const ChladniParams& params) {
        vibrationValues.resize(width * height);
        float TX = std::rand() % height;  // Random translation offset X
        float TY = std::rand() % height;  // Random translation offset Y

        // Calculate vibration values across the grid.
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float scaledX = x * params.l + TX;
                float scaledY = y * params.l + TY;
                float NX = params.n * scaledX;
                float MX = params.m * scaledX;
                float NY = params.n * scaledY;
                float MY = params.m * scaledY;

                // Vibration formula for a Chladni plate.
                float value = std::cos(NX) * std::cos(MY) - std::cos(MX) * std::cos(NY);
                value /= 2; 
                value *= std::copysign(1.0, value); 
                vibrationValues[y * width + x] = value;
            }
        }
    }

    // Computes gradients from the vibration values to guide particle movement.
    void computeGradients() {
        gradients.resize(width * height);
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 1; x < width - 1; ++x) {
                int index = y * width + x;
                float currentVibration = vibrationValues[index];
                if (std::abs(currentVibration) < 0.01) {  
                    gradients[index] = {0, 0};
                    continue;
                }

                Gradient bestGradient = {0, 0};
                float minVibration = std::numeric_limits<float>::max();

                 // Find the gradient with minimum neighboring vibration value.
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


// Particle structure for representing individual particles in the simulation.
struct Particle {
    float x, y;    // Position of the particle.
    Particle(float x, float y) : x(x), y(y) {}
};

// Function prototypes
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void initializeParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight);
void updateParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight, bool isRunning);
void renderParticles(const std::vector<Particle>& particles, int windowWidth, int windowHeight);
void initializeParticlesAtMouse(std::vector<Particle>& particles, int windowWidth, int windowHeight, int count, float posX, float posY);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
float calculateFrequency(const ChladniParams& params);
void displayFrequency(GLFWwindow* window, float frequency);

// Global variables to control simulation state.
bool isRunning = false;
bool needsResize = false;
float currentFrequency = 0.0;

// Function to handle key press events.
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_SPACE: 
            // Toggle simulation
                isRunning = !isRunning;
                break;
            case GLFW_KEY_R: 
            // Resize
                needsResize = true;
                break;
            case GLFW_KEY_UP: 
            // Increase frequency pattern
                currentParamIndex = (currentParamIndex + 1) % chladniParams.size();
                needsResize = true;
                currentFrequency = calculateFrequency(chladniParams[currentParamIndex]);
                displayFrequency(window, currentFrequency);
                break;
            case GLFW_KEY_DOWN: 
            // Decrease frequency pattern
                currentParamIndex = (currentParamIndex - 1 + chladniParams.size()) % chladniParams.size();
                needsResize = true;  
                currentFrequency = calculateFrequency(chladniParams[currentParamIndex]);
                displayFrequency(window, currentFrequency);
                break;
        }
    }
}

float calculateFrequency(const ChladniParams& params) {
    // Physical constants for steel
    const float E = 2.1e11f; // Young's modulus in Pascal
    const float density = 7800.0f; // Density in kg/m^3
    const float h = 0.01f; // Plate thickness in meters
    const float a = 0.5f; // Side length of the square plate in meters

    // Frequency calculation for a square plate with free boundaries
    float frequency = (PI / 2) * sqrt(E / density) * (h / (a * a)) * sqrt((params.m * params.m) + (params.n * params.n));
    return frequency;
}

void displayFrequency(GLFWwindow* window, float frequency) {
    char title[256];
    sprintf(title, "Chladni Plate Simulation - Frequency: %.2f Hz", frequency);
    glfwSetWindowTitle(window, title);
}

// Function to handle mouse button press events.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        ypos = windowHeight - ypos - 1;

        void* ptr = glfwGetWindowUserPointer(window);
        if (!ptr) return; 
        std::vector<Particle>* particles = static_cast<std::vector<Particle>*>(ptr);

        initializeParticlesAtMouse(*particles, windowWidth, windowHeight, 500, xpos, ypos); 
    }
}

// Function to initialize particles at a given mouse position.
void initializeParticlesAtMouse(std::vector<Particle>& particles, int windowWidth, int windowHeight, int count, float posX, float posY) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10.0, 10.0); 

    for (int i = 0; i < count; ++i) {
        float x = posX + dis(gen); 
        float y = posY + dis(gen);
        particles.emplace_back(x, y);
    }
}


// Function to initialize particles at random positions.
void initializeParticles(std::vector<Particle>& particles, int windowWidth, int windowHeight) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    particles.clear();
    for (int i = 0; i < 30000; ++i) {
        particles.emplace_back(dis(gen) * windowWidth, dis(gen) * windowHeight);
    }
}

// Function to update particle positions based on the simulation gradients.
void updateParticles(std::vector<Particle>& particles, Simulation& sim, int windowWidth, int windowHeight, bool isRunning) {
    if (!isRunning) return;

    // Slow factor to control particle movement speed.
    float slowFactor = 0.2; 

    // Update particle positions based on the gradient vectors.
    for (auto& particle : particles) {
        if (particle.x < 0 || particle.x >= windowWidth || particle.y < 0 || particle.y >= windowHeight) {
            continue; 
        }

        int index = static_cast<int>(particle.y) * windowWidth + static_cast<int>(particle.x);
        if (index < 0 || index >= sim.gradients.size()) {
            continue; 
        }

        Gradient grad = sim.gradients[index];

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-0.5, 0.5); // Random displacement distribution

// Randomly adjust the particle's position
        particle.x += grad.dx * slowFactor + dis(gen); // Add some randomness
        particle.y += grad.dy * slowFactor + dis(gen);//
        // Apply a factor to slow down movement
       // particle.x += grad.dx * slowFactor + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) * windowWidth);
       // particle.y += grad.dy * slowFactor + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) * windowHeight);

        // Boundary checks
    }
}

// Function to render particles on the screen.
void renderParticles(const std::vector<Particle>& particles, int windowWidth, int windowHeight) {
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (const auto& particle : particles) {
        if (particle.x <= 0 || particle.x >= windowWidth || particle.y <= 0 || particle.y >= windowHeight) {
            continue; 
        }
        float glX = (particle.x / windowWidth) * 2.0f - 1.0f;
        float glY = (particle.y / windowHeight) * 2.0f - 1.0f;
        glVertex2f(glX, glY);
    }
    glEnd();
}


// Main function to run the simulation.
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
    glfwSetKeyCallback(window, keyCallback); 
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    


    // Create and initialize particles
    std::vector<Particle> particles;
    initializeParticles(particles, windowWidth, windowHeight);
    glfwSetWindowUserPointer(window, &particles);


    // Initialize Simulation
    Simulation sim;
    sim.width = windowWidth;
    sim.height = windowHeight;
    sim.computeVibrationValues(chladniParams[0]);
    sim.computeGradients();
    float currentFrequency = calculateFrequency(chladniParams[currentParamIndex]);
    displayFrequency(window, currentFrequency);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Check if parameters need updating
        if (needsResize) {
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glViewport(0, 0, windowWidth, windowHeight);
            initializeParticles(particles, windowWidth, windowHeight);
            sim.width = windowWidth;
            sim.height = windowHeight;
            sim.computeVibrationValues(chladniParams[currentParamIndex]); 
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
