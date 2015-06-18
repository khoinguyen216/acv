#ifndef MD_WORKER_H
#define MD_WORKER_H

#include <vector>

#include <opencv2/opencv.hpp>

#include <QObject>

#include "target.h"


namespace BoatDetection {
class MotionDetector;
}

class md_worker : public QObject {
	Q_OBJECT

public:
	md_worker();
	~md_worker();

public slots:
	void process_frame(cv::Mat const& fr, double ts);

signals:
	void result_ready(cv::Mat const& fr);

private:
	BoatDetection::MotionDetector*		md_;
	cv::Mat				prevfr_;
	double				prevts_;
	std::vector<Target>	prevobjs_;
	unsigned long		frcount_;
};
#endif
