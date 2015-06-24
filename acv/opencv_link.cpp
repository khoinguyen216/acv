#include <opencv2/opencv.hpp>

#include <QMetaType>


void opencv_link()
{
	// Pull in highgui
	cv::Mat m(1, 1, CV_8UC1);
	cv::VideoCapture cap("");

	// Pull in video
	cv::KalmanFilter kalman;
	
	qRegisterMetaType<cv::Mat>("cv::Mat");
}
