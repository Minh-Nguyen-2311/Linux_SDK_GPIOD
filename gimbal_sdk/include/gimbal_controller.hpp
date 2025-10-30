#pragma once 
#include "pwm_bus.hpp"
#include "servo_180.hpp"
#include "vision_pipeline.hpp"

class GimbalController
{
    Servo180 servo180_pan;
    Servo180 servo180_tilt;
    float frame_x, frame_y;
public:
    GimbalController();
    ~GimbalController();
    void track(float w, float h);
    void center();
};