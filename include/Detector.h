#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>
#include "Detection.h"

struct Detector {
    cv::dnn::Net model {};
	std::vector<std::string> classNames;
	float confidenceThreshold;
    float iouThreshold;
    std::string className;
    float confidence;
    int x, y, width, height;

    Detector(const std::string& modelPath, const std::string& namesPath, 
        float confidenceThreshold = 0.5f, float iouThreshold = 0.4f);
    std::vector<Detection> detect(cv::Mat& frame);
};