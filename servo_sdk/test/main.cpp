#include "pid.hpp"
#include "servo_driver.hpp"
#include <iostream>
#include <unistd.h>

int main(void)
{
    float prev_angle = 0.0f;
    float curr_angle = 0.01f;

    servo_driver_init("gpiochip0", 12);
    
    while(1){
        std::cout << "Enter desired angle: ";
        std::cin >> curr_angle;
        if(prev_angle != curr_angle){
            servo_driver_set_angle(curr_angle);
            prev_angle = curr_angle;
        }
    }
    return 0;
}