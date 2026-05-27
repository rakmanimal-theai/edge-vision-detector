#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
	cv::VideoCapture camera(0);
	if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

	// create a window to display the images from the webcam
    cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

    // array to hold image
    cv::Mat frame;
    
    // display the frame until you press a key
    while (1) {
        // capture the next frame from the webcam
        camera >> frame;

		cv::Mat processed;

		cv::resize(frame, processed, cv::Size(640, 480));
    	cv::cvtColor(processed, processed, CV_8UC1);


        // Draw a green rectangle
        // Parameters: (image, top-left point, bottom-right point, color, thickness)
        cv::rectangle(processed, 
                      cv::Point(100, 100), 
                      cv::Point(300, 300), 
                      cv::Scalar(200, 200, 0), 
                      3);

		if (frame.empty()) {
			std::cerr << "ERROR: Empty frame received" << std::endl;
			break;
		}

		cv::Mat blurred;
    	cv::GaussianBlur(frame, blurred, cv::Size(25, 25), 0);
			
        // show the image on the window
        cv::imshow("Webcam", blurred);
        // wait (10ms) for esc key to be pressed to stop
        if (cv::waitKey(10) == 27)
            break;
    }

	return 0;
}