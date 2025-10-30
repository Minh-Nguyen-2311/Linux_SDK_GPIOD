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
    
    //Wait for rising edge 1
    while(true)
    {
        if(gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if(gpiod_line_event_read(m_pwm1, &evt) == 0 && evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
        {
            ts_rise1 = evt.ts;
            break;
        }
    }

    //Wait for falling edge
    while(true)
    {
        if(gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if(gpiod_line_event_read(m_pwm1, &evt) == 0 && evt.event_type == GPIOD_LINE_EVENT_FALLING_EDGE)
        {
            ts_fall = evt.ts;
            break;
        }
    }

    //Wait for rising edge 1
    while(true)
    {
        if(gpiod_line_event_wait(m_pwm1, nullptr) != 1) continue;
        if(gpiod_line_event_read(m_pwm1, &evt) == 0 && evt.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
        {
            ts_rise2 = evt.ts;
            break;
        }
    }

    double rise1 = ts_rise1.tv_sec + ts_rise1.tv_nsec/1e9;
    
    double rise2 = ts_rise2.tv_sec +ts_rise2.tv_nsec/1e9;

    double fall = ts_fall.tv_sec +ts_fall.tv_nsec/1e9;


    double highTime = fall - rise1;
    std::cout << "highTime: " << highTime << "\n";
    double period = rise2 - rise1;
    std::cout << "period: " << period << "\n";

    if(period <= 0) period = 1.0/910.0;
    double duty = (highTime / period) * 100.0;
    std::cout << "Duty: " << duty << "\n";

    // map duty → angle
    currentAngle = (float)((duty - 2.9f) * 360.0f/ (97.1f - 2.9f));
    std::cout << "currentAngle: " << currentAngle << "\n";
    
    // low-pass filter
    filteredAngle = 0.7 * filteredAngle + 0.3 * currentAngle;

    // compute multi-turn
    float delta = filteredAngle - lastAngle;
    if(delta > 180.0f) delta -= 360.0f;
    if(delta < -180.0f) delta += 360.0f;
    countRev += delta/360.0f;
    lastAngle = filteredAngle;
    multiRevAngle = countRev * 360.0f + filteredAngle;
    std::cout << "MultiRevAngle: " << multiRevAngle << "\n";

    return multiRevAngle;
}

void Servo360::updatePIDWithFeedback(float target, float dt) {
    float actual = readFeedbackAngle();
    // std::cout << "Actual angle: " << actual << "\n";
    float error = target - actual;

    integral += error * dt;
    float derivative = (error - prevError) / dt;
    float output = Kp * error + Ki * integral + Kd * derivative;
    prevError = error;

    // giới hạn output vào [-1..1]
    // std::cout << "Output before clamping: " << output << "\n";
    output = std::clamp(output, -1.0f, 1.0f);

    // chuyển output PID → duty servo
    float pulse_us = 1500.0f + output * 220.0f;
    float duty = pulse_us / 20000;
    
    PWM_Bus::PWM_Out(duty);

    std::cout << "[PID] target = " << target
              << "°, actual = " << actual
              << "°, err = " << error
              << ", out = " << output << std::endl;
}


