#include "gimbal_controller.hpp"

GimbalController::GimbalController()
{
    frame_x = 0.0f;
    frame_y = 0.0f;
    servo180_pan.init("gpiochip0", 12);
    servo180_tilt.init("gpiochip0", 13);
}

GimbalController::~GimbalController()
{
    frame_x = 0.0f;
    frame_y = 0.0f;
    servo180_pan.release();
    servo180_tilt.release();
}

void GimbalController::track(float w, float h)
{
    float center_w = frame_x / 2.0f;
    float center_h = frame_y / 2.0f;

    servo180_pan.set_target_angle(center_w);
    servo180_tilt.set_target_angle(center_h);
}

void GimbalController::center()
{
    servo180_pan.set_target_angle(90);
    servo180_tilt.set_target_angle(90);
}