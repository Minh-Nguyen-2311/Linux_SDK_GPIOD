#include "servo_180.hpp"
#include "servo_360.hpp"
#include <iostream>
#include <unistd.h>

int main(void)
{
    // float prev_angle = 0.0f;
    float curr_angle = 100.0f;

    // Servo180 servo180;
    // servo180.init("gpiochip0", 12);

    Servo360 servo360;
    servo360.init("gpiochip0", 14, 12);
    
    while(1)
    {
        // std::cout << "Enter desired angle: ";
        // std::cin >> curr_angle;
        servo360.updatePIDWithFeedback(curr_angle, 0.01);
    }
    return 0;
}