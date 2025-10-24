#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

class ObjectTracker {
public:
    bool init(const cv::Mat& frame, const cv::Rect& roi);
    bool update(const cv::Mat& frame, cv::Rect& roi);

private:
    cv::Ptr<cv::Tracker> tracker;
    bool active;
};
