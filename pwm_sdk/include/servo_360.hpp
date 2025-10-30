#pragma once
#include "pwm_bus.hpp"
#include <chrono>
#include <iostream>

class Servo360 : public PWM_Bus {
public:
    Servo360();
    ~Servo360();

    bool init(const char* chipname,
            unsigned int pwm0_line,
            unsigned int pwm1_line) override;

    void release() override;

    void updatePIDWithFeedback(float target, float dt);       // cập nhật PID với feedback
    float readFeedbackAngle();                  // đọc góc hiện tại từ PWM feedback
    
private:
    // PID parameters
    float Kp = 1.0f, Ki = 0.001f, Kd = 0.01f;
    float currentAngle = 0.0f, filteredAngle = 0.0f, multiRevAngle = 0.0f;
    float prevError, integral;

    // Servo calibration
    float lastAngle = 0.0f;

    float countRev = 0;

    std::chrono::steady_clock::time_point lastUpdate;
};
