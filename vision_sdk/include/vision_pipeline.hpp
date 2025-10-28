#pragma once
#include "camera_manager.hpp"
#include "object_detector.hpp"
#include "object_tracker.hpp"
#include <iostream>
#include <chrono>

class VisionPipeline {
public:
    bool init();
    bool processFrame(float& x_center, float& y_center, cv::Mat& frame);

private:
    CameraManager cam;
    ObjectDetector detector;
    ObjectTracker tracker;
    bool tracking;
    cv::Rect target;

    int frame_count = 0;
    float fps = 0.0;

    void updateFPS();
};
