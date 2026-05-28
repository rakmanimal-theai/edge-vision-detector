#include "Detector.h"
#include <iostream>
#include <fstream>
#include <vector>

Detector::Detector(const std::string& modelPath, const std::string& namesPath,
    float confidenceThreshold, float iouThreshold){
    this->confidenceThreshold = confidenceThreshold;
    this->iouThreshold = iouThreshold;
    model = cv::dnn::readNet(modelPath);
    if (model.empty()) {
        std::cerr << "ERROR: Could not load model" << std::endl;
        exit(1);
    }

    std::ifstream namesFile(namesPath);
    if (!namesFile.is_open()) {
        std::cerr << "ERROR: Could not open coco.names" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(namesFile, line)) {
        classNames.push_back(line);
    }
    std::cout << "Loaded " << classNames.size() << " class names" << std::endl;
}

std::vector<Detection> Detector::detect(cv::Mat& frame) {
    cv::Mat blob = cv::dnn::blobFromImage(
        frame, 1.0/255.0, cv::Size(640, 640),
        cv::Scalar(0, 0, 0), true, false
    );
    model.setInput(blob);

    cv::Mat outputs = model.forward();
    cv::Mat detections(outputs.size[1], outputs.size[2], CV_32F, outputs.ptr<float>());

    float scaleX = (float)frame.cols / 640.0f;
    float scaleY = (float)frame.rows / 640.0f;

    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;

    // STEP 1 — collect all candidate boxes above threshold
    // NMS needs all boxes upfront before it can decide which to keep
    for (int i = 0; i < detections.rows; i++) {
        float confidence = detections.at<float>(i, 4);

        if (confidence < confidenceThreshold) continue;

        // get class with highest score
        cv::Mat scores = detections.row(i).colRange(5, 85);
        cv::Point classIdPoint;
        double maxScore;
        cv::minMaxLoc(scores, 0, &maxScore, 0, &classIdPoint);

        // combine objectness confidence with class score
        // this is the standard YOLOv5 scoring formula
        float finalScore = confidence * (float)maxScore;
        if (finalScore < confidenceThreshold) continue;

        float cx = detections.at<float>(i, 0);
        float cy = detections.at<float>(i, 1);
        float w  = detections.at<float>(i, 2);
        float h  = detections.at<float>(i, 3);

        int x = (int)((cx - w/2) * scaleX);
        int y = (int)((cy - h/2) * scaleY);
        int width  = (int)(w * scaleX);
        int height = (int)(h * scaleY);

        // clamp to frame
        x = std::max(0, x);
        y = std::max(0, y);
        width  = std::min(width,  frame.cols - x);
        height = std::min(height, frame.rows - y);

        if (width <= 0 || height <= 0) continue;

        // store for NMS
        boxes.push_back(cv::Rect(x, y, width, height));
        confidences.push_back(finalScore);
        classIds.push_back(classIdPoint.x);
    }

    // STEP 2 — run NMS
    // 0.5 = confidence threshold (discard boxes below this)
    // 0.4 = IoU threshold (if two boxes overlap more than 40%, keep only the best one)
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, iouThreshold, indices);

    std::vector<Detection> results;

    // STEP 3 — draw only the boxes that survived NMS
    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        float conf   = confidences[idx];
        int classId  = classIds[idx];

        cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);

        std::string className = (classId < (int)classNames.size()) 
                    ? classNames[classId] 
                    : "unknown";
        std::string label = className + " " + std::to_string((int)(conf * 100)) + "%";

       Detection d;
        d.className = className;
        d.confidence = conf;
        d.x = box.x;
        d.y = box.y;
        d.width = box.width;
        d.height = box.height;
        results.push_back(d);

        // make sure label doesn't go above the frame
        int labelY = std::max(20, box.y - 10);
        cv::putText(frame, label, cv::Point(box.x, labelY),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    }

    return results;
}