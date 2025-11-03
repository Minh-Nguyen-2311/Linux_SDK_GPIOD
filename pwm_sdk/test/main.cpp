#include "servo_180.hpp"
#include "servo_360.hpp"
#include <iostream>
#include <unistd.h>

int main(void)
{
    std::cout << "Testing Parallax 360 servo\n";

    // Servo180 servo180;
    // servo180.init("gpiochip0", 12);

    Servo360 servo360;
    servo360.init("gpiochip0", 14, 12);

    float target_angle = 260.0f;
    float current_angle = 0.0f;

    while(1)
    {
        servo360.updatePIDWithFeedback(target_angle, 0.02);
        current_angle = servo360.readFeedbackAngle();
        if(target_angle - current_angle < -0.01)
            break;
    }

    std::cout << "Last recorded angle: " << current_angle << "\n";
    servo360.release();
    return 0;
}