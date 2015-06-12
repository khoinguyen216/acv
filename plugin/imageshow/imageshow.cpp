#include "imageshow.h"
#include "ui_imageshow.h"

#include <cassert>

#include <QDebug>


imageshow::imageshow()
{
	// Set up socket interfaces
	sockets_["video"].dir_out = false;
	sockets_["video"].wires["frame"] = "process_frame(cv::Mat const)";

	scene_.setBackgroundBrush(QApplication::palette().window());
}

imageshow::~imageshow()
{
}

plugin_socket const imageshow::expose_socket(QString const& s) const
{
	return sockets_[s];
}

void imageshow::start()
{
}

void imageshow::stop()
{
}

void imageshow::restart()
{
}

void imageshow::setupui(QWidget *parent)
{
	ui_ = new Ui_ImageShowUi();
	ui_->setupUi(parent);
	ui_->imageView->setScene(&scene_);
	ui_->imageView->show();
}

void imageshow::process_frame(cv::Mat const frame)
{
	if (frame.data != 0) {
		scene_.clear();
		auto const& pix = cvMat_to_QPixmap(frame);
		scene_.addPixmap(pix);
		scene_.setSceneRect(0, 0, pix.width(), pix.height());
		QRectF bounds = scene_.itemsBoundingRect();
		ui_->imageView->fitInView(bounds, Qt::KeepAspectRatio);

		ui_->widthLabel->setText(QString::number(frame.cols));
		ui_->heightLabel->setText(QString::number(frame.rows));
		ui_->channelsLabel->setText(QString::number(frame.channels()));
	}
}

QImage imageshow::cvMat_to_QImage(cv::Mat const& m)
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

QPixmap imageshow::cvMat_to_QPixmap(cv::Mat const& m)
{
	return QPixmap::fromImage(cvMat_to_QImage(m));
}
