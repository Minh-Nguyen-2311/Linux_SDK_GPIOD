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

    void updatePIDWithFeedback(float target_deg, float dt);       // cập nhật PID với feedback
    float readFeedbackAngle();                                // đọc góc hiện tại từ PWM feedback
    
private:
    // Tuning
    static constexpr float ALPHA = 0.3f;     // low-pass (smaller latency)
    static constexpr float KP = 2.3f;
    static constexpr float KI = 0.02f;
    static constexpr float KD = 0.08f;

    // From datasheet
    static constexpr double MIN_DUTY = 2.7;     // %
    static constexpr double MAX_DUTY = 97.1;    // %
    static constexpr double MIN_FEEDBACK_MS = 1.0;   // 910 Hz ~ 1.1ms; allow margins
    static constexpr double MAX_FEEDBACK_MS = 1.2;
    static constexpr double MIN_T_HIGH_MS   = 0.0297;  // ~2.7% of 1.1ms
    static constexpr double MAX_T_HIGH_MS   = 1.0681;  // generous
};
