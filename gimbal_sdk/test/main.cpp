#include "gimbal_controller.hpp"
#include <thread>
#include <iostream>

int main()
{
    GimbalController gimbal;

    std::thread t1(&GimbalController::visionThread, &gimbal);
    std::thread t2(&GimbalController::gimbalThread, &gimbal);

    std::cout << "Gimbal App starting ...\n";
    cv::Mat localFrame;

    while(1)
    {
        {
            std::lock_guard<std::mutex> lock(gimbal.g_frame_mutex);
            if(!gimbal.g_shared_frame.empty())
                gimbal.g_shared_frame.copyTo(localFrame);
        }

        if(!localFrame.empty()) cv::imshow("Gimbal Vision", localFrame);

        if(cv::waitKey(1) == 27)
        {
            std::cout << "\n[INFO] Esc pressed, shutting down...\n";
            gimbal.running.store(true);
            break;
        }
    }
    std::cout << "Exiting ...\n";

    return 0;
}