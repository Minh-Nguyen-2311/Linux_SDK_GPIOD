#include "vision_pipeline.hpp"

auto t_start = std::chrono::high_resolution_clock::now();

bool VisionPipeline::init() {
    if (!cam.open(0, 640, 480)) return false;
    if (!detector.initFaceDetector(cv::samples::findFile("/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")))
        return false;
    tracking = false;
    return true;
}

void VisionPipeline::updateFPS()
{
    frame_count++;
    auto t_now = std::chrono::high_resolution_clock::now();
    float t_elapsed = std::chrono::duration<float>(t_now - t_start).count();

    if(t_elapsed >= 1.0){
        fps = frame_count / t_elapsed;
        frame_count = 0;
        t_start = t_now;
    }
}

bool VisionPipeline::processFrame(float& x_center, float& y_center, cv::Mat& frame) {
    if (!cam.readFrame(frame)) return false;

    updateFPS();
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

    cv::putText(frame, cv::format("FPS: %.1f", fps),
                        cv::Point(10,30), cv::FONT_HERSHEY_SIMPLEX, 0.8,
                        cv::Scalar(0,255,0), 2);

    return true;
}
