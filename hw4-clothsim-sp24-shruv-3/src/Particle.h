// Particle.h
#ifndef PARTICLE_H
#define PARTICLE_H

struct Particle {
    float x, y;     // Position
    float vx, vy;   // Velocity

    Particle(float x, float y) : x(x), y(y), vx(0), vy(0) {}
};

#endif // PARTICLE_H
