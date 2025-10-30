#pragma once
#include "pwm_bus.hpp"
#include <chrono>
#include <iostream>

class Servo180 : public PWM_Bus {
public:
    Servo180();
    ~Servo180();

    bool init(const char* chipname,
              unsigned int pwm0_line);

    void release() override;
    void set_target_angle(float angle);
};


