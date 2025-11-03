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
    float readFeedbackAngle();                                // đọc góc hiện tại từ PWM feedback

    static constexpr const double FEEDBACK_FREQ = 910.0;
    static constexpr const double MIN_FEEDBACK_MS = 1.0;
    static constexpr const double MAX_FEEDBACK_MS = 1.2;
    static constexpr const double MIN_T_HIGH_MS = 0.0297;
    static constexpr const double MAX_T_HIGH_MS = 1.0681;
    static constexpr const double MIN_DUTY = 2.9;
    static constexpr const double MAX_DUTY = 97.1;
    
private:
    // PID parameters
    float Kp = 0.01f, Ki = 0.001f, Kd = 0.001f;
    float currentAngle = 0.0f, filteredAngle = 0.0f, multiRevAngle = 0.0f;
    float prevError, integral;

    // Servo calibration
    float lastAngle = 0.0f;

    float countRev = 0;

    std::chrono::steady_clock::time_point lastUpdate;
};
