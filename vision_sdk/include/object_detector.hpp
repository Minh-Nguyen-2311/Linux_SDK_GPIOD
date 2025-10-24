#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct Detection {
    cv::Rect box;
    float confidence;
    std::string label;
};

class ObjectDetector {
public:
    ObjectDetector();
    bool initFaceDetector(const std::string& cascade_path);
    bool detect(const cv::Mat& frame, std::vector<Detection>& out);

private:
    cv::CascadeClassifier face_cascade;
    bool initialized;
};