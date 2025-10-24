#pragma once
#include "camera_manager.hpp"
#include "object_detector.hpp"
#include "object_tracker.hpp"

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
};
