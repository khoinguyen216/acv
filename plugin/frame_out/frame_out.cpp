#include "frame_out.h"

#include <QDebug>
#include <QThread>

#include "video/video_sink.h"
#ifdef WITH_GUI
#include "ui_frame_out.h"
#endif


frame_out::frame_out() : acv_plugin()
{
	options_.push_back({"destination", "string",
			"Video file or rtsp://"});
}

frame_out::~frame_out()
{
#ifdef WITH_GUI
	delete ui_;
#endif
}

plugin_socket const frame_out::expose_socket(QString const& s) const
{
	return sockets_[s];
}

plugin_options const& frame_out::options() const
{
	return options_;
}

void frame_out::init()
{
	// Set up socket interfaces
	sockets_["vin"].dir_out = false;
	sockets_["vin"].wires["frame"] = "frame(cv::Mat const&, double)";

	sockets_["ctrl"].dir_out = false;
	sockets_["ctrl"].wires["destination"] = "destination(QString const&)";
	sockets_["ctrl"].wires["start"] = "start()";
	sockets_["ctrl"].wires["stop"] = "stopped()";

	sockets_["status"].dir_out = true;
	sockets_["status"].wires["started"] = "started()";
	sockets_["status"].wires["stopped"] = "stopped()";

#ifdef WITH_GUI
	ui_ = new Ui_FrameOutUi();
#endif

	workthread_ = new QThread(this);
	workthread_->start();
}

#ifdef WITH_GUI
void frame_out::setupui(QWidget* parent)
{
	ui_->setupUi(parent);

	connect(ui_->startButton, SIGNAL(clicked()), this, SLOT(start()));
	connect(ui_->stopButton, SIGNAL(clicked()), this, SLOT(stop()));
}
#endif

void frame_out::destination(QString const& v)
{
	QString trimmed = v.trimmed();
	if (trimmed.isEmpty() || destination_ == trimmed)
		return;

	destination_ = trimmed;
#ifdef WITH_GUI
	if (ui_->pathEdit->text() != destination_)
		ui_->pathEdit->setText(destination_);
#endif
}

void frame_out::start()
{
	if (sink_ != nullptr)
		return;

	if (destination_.startsWith("rtsp://")) {

	} else {
		sink_ = new video_sink(destination_);
	}

	sink_->moveToThread(workthread_);
	QMetaObject::invokeMethod(sink_, "start", Qt::QueuedConnection);
}

void frame_out::stop()
{
	QMetaObject::invokeMethod(sink_, "stop", Qt::QueuedConnection);
}
