#include "pwm_bus.hpp"
#include <thread>
#include <chrono>
#include <iostream>

PWM_Bus::PWM_Bus()
{
    PWM_Bus::m_chip = nullptr;
    PWM_Bus::m_pwm0 = nullptr;
    PWM_Bus::m_pwm1 = nullptr;
}

PWM_Bus::~PWM_Bus()
{
    release();
}

bool PWM_Bus::init(const char* chipname,
            unsigned int pwm0_line,
            unsigned int pwm1_line)
{
    int pwm_out, pwm_in;
    m_chip = gpiod_chip_open_by_name(chipname);

    if (!m_chip)
        std::cout << "Failed to open gpiochip\n";

    m_pwm0 = gpiod_chip_get_line(m_chip, pwm0_line);
    m_pwm1 = gpiod_chip_get_line(m_chip, pwm1_line);
    if (!m_pwm0 && !m_pwm1) {
        std::cout << "PWM GPIO fail\n";
        return false;
    }

    pwm_out = gpiod_line_request_output(m_pwm0, "pwm_out", 1);
    //pwm_in = gpiod_line_request_input(m_pwm1, "pwm_in");
    if(pwm_out < 0){
        std::cout << "PWM GPIO request output fail\n";
        return false;
    }

    if(gpiod_line_request_both_edges_events(m_pwm1, "pwm_in") < 0){
        std::cout << "PWM GPIO request edge events fail\n";
        return false;
    } 
    return true;
}

void PWM_Bus::release()
{
    if (m_pwm0) gpiod_line_release(m_pwm0);
    if (m_pwm1) gpiod_line_release(m_pwm1);
    if (m_chip) gpiod_chip_close(m_chip);

    m_chip = nullptr;
    m_pwm0 = m_pwm1 = nullptr;
}

void PWM_Bus::PWM_Out(float duty)
{
    // Chu kỳ PWM = 20ms
    float period_ms = 1000.0f / 50;

    // Tính toán tỉ lệ thời gian bật/tắt
    auto high_time = std::chrono::microseconds((int)(duty * 1000));
    auto low_time = std::chrono::microseconds((int)((period_ms - duty) * 1000));

    // Gửi 20 xung PWM để servo cập nhật vị trí
    for (int i = 0; i < (int)period_ms; ++i) {
        gpiod_line_set_value(m_pwm0, 1);
        std::this_thread::sleep_for(high_time);
        gpiod_line_set_value(m_pwm0, 0);
        std::this_thread::sleep_for(low_time);
    }
}

