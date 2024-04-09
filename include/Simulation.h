// Simulation.h
#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>

class ChladniParams {
public:
    int m, n;
    float l;
    ChladniParams(int m, int n, float l) : m(m), n(n), l(l) {}
};

class Simulation {
private:
    std::vector<ChladniParams> params;
    float width, height;
public:
    Simulation();
    void updateDimensions(float width, float height);
    void computeVibrationValues(const ChladniParams& params);
    void computeGradients();
};

#endif // SIMULATION_H
