#include "frame_in.h"

#include <cassert>

#include <QDebug>

#ifdef WITH_GUI
#include "ui_frame_in.h"
#endif
#include "video/video_source.h"
#include "rtsp/rtsp_source.h"

frame_in::frame_in()
{
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

#ifdef WITH_GUI
	ui_ = new Ui_FrameInUi();
#endif
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

	ui_->frameView->setScene(&scene_);
	ui_->frameView->show();
}
#endif

void frame_in::source(QString const& v)
{
	QString trimmed = v.trimmed();
	assert(!trimmed.isEmpty());

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

	} else {
		fs_ = new video_source(source_);
	}
	connect_to_source(fs_);

#ifdef WITH_GUI
	ui_->pathEdit->setText(v);
#endif
}

void frame_in::connect_to_source(frame_source* s)
{
	assert(s != 0);

	connect(this, SIGNAL(start()), s, SLOT(start()));
	connect(this, SIGNAL(stop()), s, SLOT(stop()));
	connect(s, SIGNAL(started()), this, SIGNAL(started()));
	connect(s, SIGNAL(stopped()), this, SIGNAL(stopped()));
	connect(s, SIGNAL(frame(cv::Mat const&, double)),
			this, SIGNAL(frame(cv::Mat const&, double)));
#ifdef WITH_GUI
	connect(s, SIGNAL(started()), this, SLOT(on_started()));
	connect(s, SIGNAL(stopped()), this, SLOT(on_stopped()));
	connect(s, SIGNAL(frame(cv::Mat const&, double)),
			this, SLOT(liveview(cv::Mat const&, double)));
	connect(s, SIGNAL(error(QString const&)),
			ui_->errorLabel, SLOT(setText(QString const&)));
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

void frame_in::on_started()
{
	 ui_->pathEdit->setEnabled(false);
	 ui_->startButton->setEnabled(false);
	 ui_->stopButton->setEnabled(true);
	 ui_->errorLabel->clear();
}

void frame_in::on_stopped()
{
	 ui_->pathEdit->setEnabled(true);
	 ui_->startButton->setEnabled(true);
	 ui_->stopButton->setEnabled(false);
}

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
