#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include "Stream.h"
#include "Detector.h"
#include "Detection.h"
#include <chrono>
#include <ctime>
#include <sstream>

int main()
{
	std::string filename = "../config.yaml";
	Stream videoStream;

    cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);	

	cv::FileStorage fs;
	fs.open(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		std::cerr << "Failed to open " << filename << std::endl;
		return 1;
	}

	float confidenceThreshold, iouThreshold;
	std::string modelPath, namesPath;
	fs["model_path"] >> modelPath;
	fs["names_path"] >> namesPath;
	fs["confidence_threshold"] >> confidenceThreshold;
	fs["iou_threshold"] >> iouThreshold;
	fs.release();

	Detector videoDetector = Detector(modelPath, namesPath, confidenceThreshold, iouThreshold);

	// Time counter
	auto lastTime = cv::getTickCount();
	double lastFps = 0.0;

	std::ofstream logs("../detections.log");
	
	if (!logs.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return 1;
    }

	logs << "timestamp,fps,class,confidence,x,y,width,height\n";

    while (1) {
		cv::Mat frame = videoStream.getFrame();
		if (frame.empty()) continue;
		std::vector<Detection> detections = videoDetector.detect(frame);

		std::string fpsLabel = "FPS: " + std::to_string((int)lastFps);
    	cv::putText(frame, fpsLabel, cv::Point(10, 30),
               cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);

		
		auto now = std::chrono::system_clock::now();
		std::time_t timeT = std::chrono::system_clock::to_time_t(now);

		std::ostringstream oss;
		oss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
		std::string timestamp = oss.str();
		
		for (const Detection& d : detections) {
			logs << timestamp << ","
				<< (int)lastFps << ","
				<< d.className << ","
				<< d.confidence << ","
				<< d.x << ","
				<< d.y << ","
				<< d.width << ","
				<< d.height << "\n";
		}
        cv::imshow("Webcam", frame);

		lastFps = cv::getTickFrequency() / (cv::getTickCount() - lastTime);
    	lastTime = cv::getTickCount();

        if (cv::waitKey(10) == 27)
            break;
    }

	logs.close();
	std::cout << "Logs file created successfully." << std::endl;

	cv::destroyAllWindows();
    return 0;
}