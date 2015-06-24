#include "md_worker.h"

#include <vector>

#include <QDebug>

#include "MotionDetection.h"


using std::vector;
using cv::Mat;
using cv::Point;
using cv::Scalar;

md_worker::md_worker() :
	prevts_(0),
	frcount_(0)
{
	md_ = new BoatDetection::MotionDetector(3, 0, 360, 0.5, 90, 20*20, 0);
	prevfr_ = cv::Mat::ones(120, 160, CV_8UC3);
}

md_worker::~md_worker()
{
	delete md_;
}

void md_worker::process_frame(cv::Mat const& fr, double ts)
{
	frcount_++;
	if (frcount_ % 5 != 0)
		return;

	vector<Target> objs;

	ts /= 1000;
	cv::Mat resized;
	cv::resize(fr, resized, cv::Size(160, 120));

	md_->MotionDetection(prevfr_, resized, objs, prevts_, ts, true);
	md_->MotionCorrection(prevobjs_, objs, prevts_, ts);

	prevfr_ = resized;
	prevts_ = ts;
	prevobjs_.clear();
	prevobjs_ = objs;

	cv::Mat result = md_->GetMaskVisual();
	cv::cvtColor(result, result, CV_GRAY2BGR);
	for (auto const& o : objs) {
		cv::rectangle(result, o.bb, CV_RGB(255, 255, 0));
	}

	emit result_ready(result);
}
