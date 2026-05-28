#include "Stream.h"
#include <iostream>

Stream::Stream() : camera(0) {
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        exit(1);
    }
    fps = (int)camera.get(cv::CAP_PROP_FPS);
}

cv::Mat Stream::getFrame() {
    cv::Mat frame;
    camera >> frame;
    if (frame.empty()) {
        std::cerr << "WARNING: Empty frame received" << std::endl;
    }
    return frame;
}