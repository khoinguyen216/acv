#include "frame_in.h"

#include <cassert>

#include <QDebug>
#include <QThread>

#ifdef WITH_GUI
#include "ui_frame_in.h"
#endif
#include "http_mjpg/http_mjpg_source.h"
#include "rtsp/rtsp_source.h"
#include "video/video_source.h"

frame_in::frame_in()
{
	options_.push_back({"source", "string",
			"Media file, http:// for http/mjpg, rtsp:// for rtsp/h264"});
	options_.push_back({"recover_on_error", "boolean",
			"Perform recovery on error"});
	options_.push_back({"recover_interval", "number",
			"Interval between recovery attempts"});
	options_.push_back({"read_timeout", "number",
			"The number of seconds to trigger an error since the last frame"});
}

frame_in::~frame_in()
{
#ifdef WITH_GUI
	if (ui_)
		delete ui_;
#endif
}

plugin_socket const frame_in::expose_socket(QString const& s) const
{
	 return sockets_[s];
}

plugin_options const& frame_in::options() const
{
	return options_;
}

void frame_in::init()
{
	// Set up socket interfaces
	sockets_["vout"].dir_out = true;
	sockets_["vout"].wires["frame"] = "frame(cv::Mat const&, double)";
	sockets_["vout"].wires["started"] = "started()";
	sockets_["vout"].wires["stopped"] = "stopped";

	sockets_["ctrl"].dir_out = false;
	sockets_["ctrl"].wires["start"] = "start()";
	sockets_["ctrl"].wires["stop"] = "stop()";

	sockets_["status"].dir_out = true;
	sockets_["status"].wires["error"] =
		"error(QTime const& ts, QString const&)";

#ifdef WITH_GUI
	ui_ = new Ui_FrameInUi();
#endif

	workthread_ = new QThread(this);
	workthread_->start();

	recover_ = false;
	read_deadline_ = new QTimer(this);
	connect(read_deadline_, SIGNAL(timeout()), this, SLOT(on_read_timeout()));
	recover_timer_ = new QTimer(this);
}

#ifdef WITH_GUI
void frame_in::setupui(QWidget* parent)
{
	ui_->setupUi(parent);
	emit on_stopped();

	connect(ui_->startButton, SIGNAL(clicked()), this, SIGNAL(start()));
	connect(ui_->stopButton, SIGNAL(clicked()), this, SIGNAL(stop()));
	connect(ui_->pathEdit, SIGNAL(textChanged(QString)),
			this, SLOT(source(QString const&)));
	connect(ui_->autoRecoverCheck, SIGNAL(toggled(bool)),
			this, SLOT(recover_on_error(bool)));

	ui_->frameView->setScene(&scene_);
	ui_->frameView->show();
}
#endif

void frame_in::source(QString const& v)
{
	QString trimmed = v.trimmed();

	if (trimmed.isEmpty())
		return;

	if (source_ == trimmed)
		return;

	source_ = trimmed;

	if (fs_ != 0) {
		delete fs_;
		fs_ = 0;
	}

	if (v.startsWith("rtsp://")) {
		fs_ = new rtsp_source(source_);
	} else if (v.startsWith("http://")) {
		fs_ = new http_mjpg_source(source_);
	} else {
		fs_ = new video_source(source_);
	}
	fs_->moveToThread(workthread_);
	connect_to_source(fs_);

#ifdef WITH_GUI
	if (ui_->pathEdit->text() != source_)
		ui_->pathEdit->setText(v);
#endif
}

void frame_in::recover_on_error(bool v)
{
	if (recover_ != v) {
		recover_ = v;
#ifdef WITH_GUI
		ui_->autoRecoverCheck->setChecked(recover_);
#endif
	}
}

void frame_in::recover_interval(double v)
{
	if (recover_interval_ != v) {
		recover_interval_ = v;
		recover_timer_->setInterval(v * 1000);
#ifdef WITH_GUI
		ui_->recoverIntervalLabel->setText(QString("%1 seconds").arg(v));
#endif
	}
}

void frame_in::read_timeout(double v)
{
	if (read_timeout_ != v) {
		read_timeout_ = v;
		read_deadline_->setInterval(read_timeout_ * 1000);
#ifdef WITH_GUI
		ui_->readTimeoutLabel->setText(QString("%1 seconds").arg(v));
#endif
	}
}

void frame_in::on_started()
{
	read_deadline_->start();
#ifdef WITH_GUI
	ui_->pathEdit->setEnabled(false);
	ui_->startButton->setEnabled(false);
	ui_->stopButton->setEnabled(true);
	ui_->errorLabel->clear();
#endif
}

void frame_in::on_stopped()
{
	read_deadline_->stop();
#ifdef WITH_GUI
	ui_->pathEdit->setEnabled(true);
	ui_->startButton->setEnabled(true);
	ui_->stopButton->setEnabled(false);
	scene_.clear();
#endif
}

void frame_in::on_frame(cv::Mat const& f, double ts)
{
	emit frame(f, ts);
	read_deadline_->start();
}

void frame_in::on_error(QDateTime const& ts, QString e)
{
	emit stop();
	emit error(ts, e);
	read_deadline_->stop();
	if (recover_) {
		recover_timer_->singleShot(recover_interval_ * 1000,
									this, SIGNAL(start()));
	}
#ifdef WITH_GUI
	ui_->errorLabel->setText(QString("%1 %2").arg(ts.toString()).arg(e));
#endif
}

void frame_in::on_read_timeout()
{
	on_error(QDateTime::currentDateTime(), "Read timeout");
}

void frame_in::connect_to_source(frame_source* s)
{
	assert(s != 0);

	connect(this, SIGNAL(start()), s, SLOT(start()));
	connect(this, SIGNAL(stop()), s, SLOT(stop()));
	connect(s, SIGNAL(started()), this, SLOT(on_started()));
	connect(s, SIGNAL(stopped()), this, SLOT(on_stopped()));
	connect(s, SIGNAL(frame(cv::Mat const&, double)),
			this, SLOT(on_frame(cv::Mat const&, double)));
	connect(s, SIGNAL(error(QDateTime const&, QString const&)),
			this, SLOT(on_error(QDateTime const&, QString const&)));
#ifdef WITH_GUI
	connect(s, SIGNAL(frame(cv::Mat const&, double)),
			this, SLOT(liveview(cv::Mat const&, double)));
#endif
}

#ifdef WITH_GUI
void frame_in::liveview(cv::Mat const& f, double ts)
{
	if (ui_->liveViewCheck->isChecked()) {
		auto const& pix = cvMat_to_QPixmap(f);
		scene_.clear();
		scene_.addPixmap(pix);
		scene_.setSceneRect(0, 0, pix.width(), pix.height());
		QRectF bounds = scene_.itemsBoundingRect();
		ui_->frameView->fitInView(bounds, Qt::KeepAspectRatio);

		ui_->resolutionLabel->setText(QString("%1 x %2")
										.arg(f.cols).arg(f.rows));
		ui_->channelsLabel->setText(QString::number(f.channels()));
		ui_->frameLabel->setText(QString::number(ts, 'f', 2));
	}
}
#endif

#ifdef WITH_GUI
QImage frame_in::cvMat_to_QImage(cv::Mat const& m)
{
	switch (m.type())
	{
	case CV_8UC3:
	{
		QImage qimage(m.data, m.cols, m.rows, m.step, QImage::Format_RGB888);
		return qimage.rgbSwapped();
	}
	default:
		return QImage();
	}
}

QPixmap frame_in::cvMat_to_QPixmap(cv::Mat const& m)
{
	return QPixmap::fromImage(cvMat_to_QImage(m));
}
#endif
