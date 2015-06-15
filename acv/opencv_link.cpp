#include <opencv2/opencv.hpp>

void opencv_link()
{		
	cv::Mat m;
	cv::cvtColor(m, m, cv::COLOR_BGR2GRAY);
	cv::imshow("", m);
}
