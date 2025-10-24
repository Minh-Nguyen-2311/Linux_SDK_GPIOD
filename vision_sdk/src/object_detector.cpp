#include "object_detector.hpp"
#include <iostream>

ObjectDetector::ObjectDetector() : initialized(false) {}

bool ObjectDetector::initFaceDetector(const std::string& cascade_path) {
    if (!face_cascade.load(cascade_path)) {
        std::cerr << "Failed to load Haar Cascade: " << cascade_path << std::endl;
        return false;
    }
    initialized = true;
    return true;
}

bool ObjectDetector::detect(const cv::Mat& frame, std::vector<Detection>& out) {
    if (!initialized) return false;

    std::vector<cv::Rect> faces;
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);
    face_cascade.detectMultiScale(gray, faces);

    out.clear();
    for (auto& f : faces) {
        Detection d;
        d.box = f;
        d.confidence = 1.0f;
        d.label = "face";
        out.push_back(d);
    }
    return true;
}
