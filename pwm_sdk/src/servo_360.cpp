#include "servo_360.hpp"
#include <cmath>
#include <thread>
#include <algorithm>

Servo360::Servo360(){}

Servo360::~Servo360()
{
    Servo360::release(); 
}

bool Servo360::init(const char* chipname,
            unsigned int pwm0_line,
            unsigned int pwm1_line)
{
    if (!PWM_Bus::init(chipname, pwm0_line, pwm1_line)) {
        std::cerr << "Parallax Servo failed to initialize PWM bus\n";
        return false;
    }
    std::cout << " Parallax Servo initialized (PWM_Output=" << pwm0_line
              << ", PWM_Input=" << pwm1_line << ")\n";
    return true;
}

void Servo360::release()
{
    PWM_Bus::release();
}

float Servo360::readFeedbackAngle() {
    struct gpiod_line_event evt;
    struct timespec ts_rise1, ts_fall, ts_rise2;
    
    // rising edge 1
    while(true) {
        if(gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if(!gpiod_line_event_read(m_pwm1, &evt) && evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
            ts_rise1 = evt.ts;
            break;
        }
    }

    // falling edge
    while(true) {
        if(gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if(!gpiod_line_event_read(m_pwm1, &evt) && evt.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) {
            ts_fall = evt.ts;
            break;
        }
    }

    // rising edge 2
    while(true) {
        if(gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if(!gpiod_line_event_read(m_pwm1, &evt) && evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
            ts_rise2 = evt.ts;
            break;
        }
    }

    double rise1 = ts_rise1.tv_sec * 1e3 + ts_rise1.tv_nsec / 1e6;
    double fall  = ts_fall .tv_sec * 1e3 + ts_fall .tv_nsec / 1e6;
    double rise2 = ts_rise2.tv_sec * 1e3 + ts_rise2.tv_nsec / 1e6;

    double highTime = fall - rise1;
    highTime = std::clamp(highTime, MIN_T_HIGH_MS, MAX_T_HIGH_MS);

    double cycleTime = rise2 - rise1;
    cycleTime = std::clamp(cycleTime, MIN_FEEDBACK_MS, MAX_FEEDBACK_MS);

    double duty = (highTime / cycleTime) * 100.0;
    duty = std::clamp(duty, MIN_DUTY, MAX_DUTY);

    // Correct angle mapping
    currentAngle = (float)((duty - MIN_DUTY) * 360.0 / (MAX_DUTY - MIN_DUTY));

    // Low-pass filter
    filteredAngle = 0.9f * filteredAngle + 0.1f * currentAngle;

    // Multi-turn tracking
    float delta = filteredAngle - lastAngle;
    if(delta > 180.0f) delta -= 360.0f;
    if(delta < -180.0f) delta += 360.0f;

    countRev += delta / 360.0f;
    lastAngle = filteredAngle;

    multiRevAngle = countRev * 360.0f + filteredAngle;

    return filteredAngle; // <== quay về CHỈ góc 0..360 đã lọc
}

void Servo360::updatePIDWithFeedback(float target, float dt) {
    float angle = readFeedbackAngle();

    float targetWrapped = fmod(target, 360.0f);
    if (targetWrapped < 0) targetWrapped += 360.0f;

    // wrap error to [-180..180]
    float error = targetWrapped - angle;
    if(error > 180.0f) error -= 360.0f;
    if(error < -180.0f) error += 360.0f;

    integral += error * dt;
    float derivative = (error - prevError) / dt;
    prevError = error;

    float output = Kp * error + Ki * integral + Kd * derivative;
    output = std::clamp(output, -1.0f, 1.0f);

    float pulse_us = 1500.0f + output * 220.0f;
    float duty = pulse_us / 20000.0f;

    PWM_Bus::PWM_Out(duty);

    std::cout << "[PID] target=" << target
              << " actual=" << angle
              << " err=" << error
              << " pulse_us=" << pulse_us
              << std::endl;
}


