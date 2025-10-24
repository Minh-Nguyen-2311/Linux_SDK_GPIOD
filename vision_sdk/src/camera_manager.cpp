#include "camera_manager.hpp"
#include <iostream>

CameraManager::CameraManager() {}
CameraManager::~CameraManager() { close(); }

bool CameraManager::open(int index, int width, int height) {
    cap.open(index, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera index " << index << std::endl;
        return false;
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FPS, 15);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
    std::cout << "Camera opened: " << width << "x" << height << std::endl;
    return true;
}

bool CameraManager::readFrame(cv::Mat& frame) {
    return cap.read(frame);
}

void CameraManager::close() {
    if (cap.isOpened()) cap.release();
}