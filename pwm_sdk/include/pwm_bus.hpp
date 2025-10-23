#pragma once
#include <gpiod.h>
#include <cstdint>
#include <string>
#include <unistd.h>

class PWM_Bus{
public:
    PWM_Bus();
    ~PWM_Bus();

    virtual bool init(const char* chipname,
            unsigned int pwm0_line, unsigned int pwm1_line,
            unsigned int delay_us);
    virtual void release();

    void PWM_Out(float duty);
    
protected:
    struct gpiod_chip* m_chip;
    
    struct gpiod_line* m_pwm0; //Output PWM to control servo
    struct gpiod_line* m_pwm1; //(Optional) Input PWM for position feedback

    unsigned int m_delayUs;
};