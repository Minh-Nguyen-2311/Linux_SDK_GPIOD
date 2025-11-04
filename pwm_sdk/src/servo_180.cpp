#include "servo_180.hpp"
#include <cmath>
#include <iostream>

// No change in interface, but PWM_Out is now non-blocking, continuous

Servo180::Servo180(){}

Servo180::~Servo180()
{
    Servo180::release(); 
}

bool Servo180::init(const char* chipname,
                    unsigned int pwm0_line)
{
    // Reuse PWM_Bus init with dummy input line = pwm0_line (not used here)
    m_chip = gpiod_chip_open_by_name(chipname);
    if (!m_chip) {
        std::cout << "Failed to open gpiochip\n";
        return false;
    }

    m_pwm0 = gpiod_chip_get_line(m_chip, pwm0_line);
    if (!m_pwm0) {
        std::cout << "PWM GPIO fail\n";
        return false;
    }

    if (gpiod_line_request_output(m_pwm0, "servo_driver", 0) < 0) {
        std::cout << "PWM GPIO request output fail\n";
        return false;
    }

    // Start continuous PWM via PWM_Bus mechanism
    // Trick: borrow PWM_Bus thread by calling PWM_Out once to spawn if needed
    PWM_Bus::PWM_Out(1500.0f); // center

    return true;
}

void Servo180::release()
{
    PWM_Bus::release();
}

void Servo180::set_target_angle(float angle)
{
    if (angle < 0.0f) angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    // Typical 0.5ms..2.5ms on many hobby servos
    const float duty_min_ms = 0.5f;
    const float duty_max_ms = 2.5f;
    float duty_ms = duty_min_ms + (angle / 180.0f) * (duty_max_ms - duty_min_ms);

    // Non-blocking: just set pulse width, background thread keeps outputting
    PWM_Bus::PWM_Out(duty_ms); 
}
