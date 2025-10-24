#include "vision_pipeline.hpp"
#include <iostream>

bool VisionPipeline::init() {
    if (!cam.open(0, 640, 480)) return false;
    if (!detector.initFaceDetector(cv::samples::findFile("/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")))
        return false;
    tracking = false;
    return true;
}

bool VisionPipeline::processFrame(float& x_center, float& y_center, cv::Mat& frame) {
    if (!cam.readFrame(frame)) return false;

    if (!tracking) {
        std::vector<Detection> detections;
        detector.detect(frame, detections);
        if (!detections.empty()) {
            target = detections[0].box;
            tracker.init(frame, target);
            tracking = true;
        }
    } else {
        cv::Rect roi;
        if (tracker.update(frame, roi)) {
            target = roi;
        } else {
            tracking = false;
        }
    }

    if (tracking) {
        x_center = target.x + target.width / 2.0f;
        y_center = target.y + target.height / 2.0f;
        cv::rectangle(frame, target, cv::Scalar(0, 255, 0), 2);
    }

    return true;
}
