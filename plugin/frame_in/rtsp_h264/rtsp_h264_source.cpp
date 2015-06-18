#include "rtsp_h264_source.h"

#include <cassert>

#include <QDebug>
#include <QTimer>

rtsp_h264_source::rtsp_h264_source(QString const& s) : frame_source(s),
	timer_(new QTimer(this))
{
	assert(!s.isEmpty());

	connect(timer_, SIGNAL(timeout()), this, SLOT(next()));
}

rtsp_h264_source::~rtsp_h264_source()
{
	//delete cap_;
}

void rtsp_h264_source::start()
{
	//if (cap_ != 0)
		//return;

	//cap_ = new MjpgCapture(source_.toStdString());
	//try {
		//cap_->init();
		//double fps = DEFAULT_FPS;
		//timer_->setInterval(1000 / fps);
		//timer_->start();
		//start_ts_.start();
		//emit started();
	//} catch (std::string const& e) {
		//emit error(QDateTime::currentDateTime(), QString::fromStdString(e));
	//}
}

void rtsp_h264_source::stop()
{
	//if (cap_) {
		 //timer_->stop();
		 //delete cap_; cap_ = 0;
		 //emit stopped();
	//}
}

void rtsp_h264_source::next()
{
	//assert(cap_ != 0);

	//cv::Mat f;
	//try {
		//*cap_ >> f;

		//if (f.empty()) {
			//emit error(QDateTime::currentDateTime(), "Frame is empty");
			//return;
		//}

		//double ts = start_ts_.elapsed() / 1000.0;
		//emit frame(f, ts);
	//} catch (std::string const& e) {
		 //emit error(QDateTime::currentDateTime(), QString::fromStdString(e));
	//}
}
