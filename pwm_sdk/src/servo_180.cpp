#include "servo_180.hpp"
#include <cmath>
#include <thread>

Servo180::Servo180(){}

Servo180::~Servo180()
{
    Servo180::release(); 
}

bool Servo180::init(const char* chipname,
              unsigned int pwm0_line)
{
    m_chip = gpiod_chip_open_by_name(chipname);

    if (!m_chip)
        std::cout << "Failed to open gpiochip\n";

    m_pwm0 = gpiod_chip_get_line(m_chip, pwm0_line);
    if (!m_pwm0) {
        std::cout << "PWM GPIO fail\n";
        return false;
    }

    if(gpiod_line_request_output(m_pwm0, "servo_driver", 0) < 0)
    {
        std::cout << "PWM GPIO request output fail\n";
        return false;
    }
    return true;
}

void Servo180::release()
{
    PWM_Bus::release();
}

void Servo180::set_target_angle(float angle)
{
    // Giới hạn góc hợp lệ
    if (angle < 0.0f) angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    // Servo tiêu chuẩn: 1ms (0°) → 2ms (180°)
    float duty_min = 0.5f;   // ms
    float duty_max = 2.5f;   // ms
    float duty_ms = duty_min + (angle / 180.0f) * (duty_max - duty_min);
    PWM_Bus::PWM_Out(duty_ms); 
}
/*------------------------------------------------------*/


