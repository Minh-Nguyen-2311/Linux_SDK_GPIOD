#pragma once 
#include "pwm_bus.hpp"
#include "servo_180.hpp"
#include "vision_pipeline.hpp"
#include <thread>
#include <atomic>

class GimbalController
{
    Servo180 servo180_pan;
    Servo180 servo180_tilt;
    
public:
    GimbalController();
    ~GimbalController();
    void visionThread();
    void gimbalThread();

    cv::Mat g_shared_frame;
    std::mutex g_frame_mutex;
    float frame_x, frame_y;
    std::atomic<bool> running{true};
private:
    void track(float w, float h);
    void center();
};