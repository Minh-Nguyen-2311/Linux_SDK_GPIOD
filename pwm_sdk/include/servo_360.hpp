#pragma once
#include "pwm_bus.hpp"
#include <chrono>
#include <iostream>

class Servo360 : public PWM_Bus {
public:
    Servo360();
    ~Servo360();

    bool init(const char* chipname,
              unsigned int pwm0_line, unsigned int pwm1_line,
              unsigned int delay_us) override;

    void release() override;

    void setTargetAngle(float target_deg);      // mục tiêu góc (0–360)
    void updatePIDWithFeedback(float dt);       // cập nhật PID với feedback
    float readFeedbackAngle();                  // đọc góc hiện tại từ PWM feedback
    
private:
    // PID parameters
    float Kp, Ki, Kd;
    float targetAngle, currentAngle;
    float prevError, integral;

    // Servo calibration
    float center_us{1500.0f};
    float range_us{220.0f};

    std::chrono::steady_clock::time_point lastUpdate;
};