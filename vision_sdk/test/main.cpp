#include "vision_pipeline.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    VisionPipeline vision;
    if (!vision.init()) {
        std::cerr << "Failed to init vision pipeline\n";
        return -1;
    }

    cv::Mat frame;
    while (true) {
        float x, y;
        if (!vision.processFrame(x, y, frame)) continue;

        std::cout << "Target at (" << x << ", " << y << ")\n";
        cv::imshow("Vision SDK Demo", frame);
        if (cv::waitKey(10) == 27) break;
    }

    return 0;
}
