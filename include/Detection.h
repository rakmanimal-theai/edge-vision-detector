#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>

struct Detection {
    std::string className;
    float confidence;
    int x, y, width, height;
};