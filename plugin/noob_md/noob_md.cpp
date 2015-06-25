#include "noob_md.h"
#include "md_worker.h"
#ifdef WITH_GUI
#include "ui_noob_md.h"
#endif

#include <cassert>
#include <algorithm>

#include <QDebug>
#include <QRgb>
#include <QThread>
#include <QVector>


noob_md::noob_md()
{
}

noob_md::~noob_md()
{
	if (worker_thread_)
		delete worker_thread_;
	if (worker_)
		delete worker_;
#ifdef WITH_GUI
	if (ui_)
		delete ui_;
#endif
}

plugin_socket const noob_md::expose_socket(QString const& s) const
{
	 return sockets_[s];
}

plugin_options const& noob_md::options() const
{
	 return options_;
}

void noob_md::init()
{
	// Set up socket interfaces
	sockets_["video"].dir_out = false;
	sockets_["video"].wires["frame"] = "process_frame(cv::Mat const&, double)";

	sockets_["vidctrl"].dir_out = true;
	sockets_["vidctrl"].wires["start"] = "start_video()";
	sockets_["vidctrl"].wires["stop"] = "stop_video()";

#ifdef WITH_GUI
	ui_ = new Ui_NoobMdUi();
#endif

	qRegisterMetaType<cv::Mat>("cv::Mat");

	worker_ = new md_worker();
	worker_thread_ = new QThread();
	worker_->moveToThread(worker_thread_);
	connect(worker_, SIGNAL(result_ready(cv::Mat const&)),
			this, SLOT(visualize_result(cv::Mat const&)));
	worker_thread_->start();
}

#ifdef WITH_GUI
void noob_md::setupui(QWidget* parent)
{
	ui_->setupUi(parent);

	ui_->frameView->setScene(&scene_);
	ui_->frameView->show();
	connect(ui_->startButton, SIGNAL(clicked()), this, SIGNAL(start_video()));
	connect(ui_->stopButton, SIGNAL(clicked()), this, SIGNAL(stop_video()));
}
#endif

void noob_md::process_frame(cv::Mat const& fr, double ts)
{
	QMetaObject::invokeMethod(worker_, "process_frame", Qt::QueuedConnection,
			Q_ARG(cv::Mat const&, fr), Q_ARG(double, ts));
}

#ifdef WITH_GUI
void noob_md::visualize_result(cv::Mat const& fr)
{
	auto const& pix = cvMat_to_QPixmap(fr);
	scene_.clear();
	scene_.addPixmap(pix);
	scene_.setSceneRect(0, 0, pix.width(), pix.height());
	QRectF bounds = scene_.itemsBoundingRect();
	ui_->frameView->fitInView(bounds, Qt::KeepAspectRatio);
}
#endif

#ifdef WITH_GUI
QImage noob_md::cvMat_to_QImage(cv::Mat const& m)
{
	switch (m.type())
	{
	case CV_8UC3: {
		QImage qimage(m.data, m.cols, m.rows, m.step, QImage::Format_RGB888);
		return qimage.rgbSwapped();
	}
	default:
		return QImage();
	}
}

QPixmap noob_md::cvMat_to_QPixmap(cv::Mat const& m)
{
	return QPixmap::fromImage(cvMat_to_QImage(m));
}
#endif
