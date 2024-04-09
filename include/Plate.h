#ifndef PLATE_H
#define PLATE_H

class Plate {
public:
    Plate(float width, float height);
    void setFrequency(float frequency);
    void calculateVibrationPattern();
    // Additional methods for rendering or other purposes

private:
    float width, height;
    float frequency;
};

#endif // PLATE_H
