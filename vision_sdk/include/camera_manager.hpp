#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>

class CameraManager
{
public:
    CameraManager();
    ~CameraManager();

    bool open(int idx, int width, int height);
    bool readFrame(cv::Mat& frame);
    void close();
private:
    cv::VideoCapture cap;
};