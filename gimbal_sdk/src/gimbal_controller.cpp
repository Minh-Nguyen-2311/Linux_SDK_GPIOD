#include "gimbal_controller.hpp"

std::atomic<float> g_x{-1}, g_y{-1};
std::atomic<bool> g_target_ok{false};

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

void GimbalController::visionThread()
{
    VisionPipeline vision;
    if(!vision.init())
    {
        std::cout << "Failed to init vision pipeline\n";
        return;
    }
    cv::Mat frame;

    while(running.load())
    {
        float x,y;
        if(vision.processFrame(x,y,frame))
        {
            g_x.store(x);
            g_y.store(y);
            g_target_ok.store(true);
            
            {
                std::lock_guard<std::mutex> lock(g_frame_mutex);
                frame.copyTo(g_shared_frame);
            }
        }
    }
}

void GimbalController::gimbalThread()
{
    center();

    while(running.load())
    {
        if(!g_target_ok.load()) continue;

        float x = g_x.load();
        float y = g_y.load();

        track(x,y);

        std::this_thread::sleep_for(std::chrono::milliseconds(5)); //200Hz
    }
}