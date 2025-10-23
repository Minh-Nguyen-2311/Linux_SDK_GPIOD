#include "servo_360.hpp"
#include <cmath>
#include <thread>

Servo360::Servo360(){}

Servo360::~Servo360()
{
    Servo360::release(); 
}

void Servo360::release()
{
    PWM_Bus::release();
}

void Servo360::setTargetAngle(float target_deg) {
    if (target_deg < 0) target_deg = 0;
    if (target_deg > 360) target_deg = 360;
    targetAngle = target_deg;
}

float Servo360::readFeedbackAngle() {
    // đo PWM feedback bằng libgpiod
    // - ghi thời gian rising edge và falling edge
    // - tính duty cycle = high_time / period_time

    struct timespec ts_rise{}, ts_fall{};
    bool last = gpiod_line_get_value(m_pwm1);
    bool rising_found = false, falling_found = false;

    // chờ rising edge
    for (;;) {
        bool val = gpiod_line_get_value(m_pwm1);
        if (!last && val) {
            clock_gettime(CLOCK_MONOTONIC, &ts_rise);
            rising_found = true;
        }
        last = val;
        if (rising_found) break;
    }

    // chờ falling edge
    for (;;) {
        bool val = gpiod_line_get_value(m_pwm1);
        if (last && !val) {
            clock_gettime(CLOCK_MONOTONIC, &ts_fall);
            falling_found = true;
        }
        last = val;
        if (falling_found) break;
    }

    double rise_t = ts_rise.tv_sec + ts_rise.tv_nsec / 1e9;
    double fall_t = ts_fall.tv_sec + ts_fall.tv_nsec / 1e9;
    double high_time = fall_t - rise_t;          // giây
    double period = 1.0 / 910.0;                 // feedback PWM ~910 Hz
    float duty = (float)((high_time / period) * 100.0); // %

    // clamp duty
    if (duty < 2.9f) duty = 2.9f;
    if (duty > 97.1f) duty = 97.1f;

    // map duty → angle
    currentAngle = ((duty - 2.9f) / (97.1f - 2.9f)) * 360.0f;
    return currentAngle;
}

void Servo360::updatePIDWithFeedback(float dt) {
    float actual = readFeedbackAngle();

    float error = targetAngle - actual;
    // Xử lý wrap-around 0–360°
    if (error > 180.0f) error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    integral += error * dt;
    float derivative = (error - prevError) / dt;
    float output = Kp * error + Ki * integral + Kd * derivative;
    prevError = error;

    // giới hạn output vào [-1..1]
    if (output > 1.0f) output = 1.0f;
    if (output < -1.0f) output = -1.0f;

    // chuyển output PID → duty servo
    float pulse_us = center_us + output * range_us;
    float duty = pulse_us / 20000;
    PWM_Bus::PWM_Out(duty);

    std::cout << "[PID] target = " << targetAngle
              << "°, actual = " << actual
              << "°, err = " << error
              << ", out = " << output << std::endl;
}