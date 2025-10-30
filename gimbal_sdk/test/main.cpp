#include "gimbal_controller.hpp"
#include <iostream>

int main()
{
    VisionPipeline vision;
    GimbalController gimbal;
    if(!vision.init()){
        std::cerr << "Failed to init vision pipeline\n";
        return -1;
    }
    cv::Mat frame;
    cv::namedWindow("Gimbal Vision", cv::WINDOW_AUTOSIZE);
    gimbal.center();

    while(1)
    {
        float x = -1, y = -1;
        if(!vision.processFrame(x,y,frame)) continue;
        if(x>0 && y>0)
        {
            gimbal.track(x,y);
            cv::circle(frame, cv::Point(x,y), 5,
                        cv::Scalar(0,255,0), -1);
        }
        cv::imshow("Gimbal Vision", frame);
        if(cv::waitKey(10) == 27)
        {
            gimbal.center();
            break;
        }
    }
    cv::destroyAllWindows();
    return 0;
}