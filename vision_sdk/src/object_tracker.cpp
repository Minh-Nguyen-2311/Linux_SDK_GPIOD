#include "object_tracker.hpp"

bool ObjectTracker::init(const cv::Mat& frame, const cv::Rect& roi) {
    tracker = cv::TrackerCSRT::create();
    tracker->init(frame, roi);
    return active;
}

bool ObjectTracker::update(const cv::Mat& frame, cv::Rect& roi) {
    if (!active) return false;
    return tracker->update(frame, roi);
}
