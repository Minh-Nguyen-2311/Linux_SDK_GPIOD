#include "servo_180.hpp"
#include "servo_360.hpp"
#include <iostream>
#include <unistd.h>

int main(void)
{
    float prev_angle = 0.0f;
    float curr_angle = 0.01f;

    Servo180 servo180;

    servo180.init("gpiochip0", 12);
    
    while(1){
        std::cout << "Enter desired angle: ";
        std::cin >> curr_angle;
        if(prev_angle != curr_angle){
            servo180.set_target_angle(curr_angle);
            prev_angle = curr_angle;
        }
    }
    return 0;
}