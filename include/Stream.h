#pragma once
#include <opencv2/opencv.hpp>

struct Stream {
    int fps;
    cv::VideoCapture camera;

    Stream();
    cv::Mat getFrame();
};