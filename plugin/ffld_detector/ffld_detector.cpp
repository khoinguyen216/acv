#include "ffld_detector.h"

#include <cassert>

#include <QDebug>
#include <QFileDialog>

#include <opencv2/opencv.hpp>

#include "ui_ui.h"


ffld_detector::ffld_detector()
{
	// Set up socket interfaces
	sockets_["video"].dir_out = false;
	sockets_["video"].wires["frame"] = "process_frame(cv::Mat const)";

	// Set up connections with the detector
	connect(&detector_, SIGNAL(configure_status(QString const&)),
			this, SLOT(on_configure_status(QString const&)));
}

ffld_detector::~ffld_detector()
{
}

plugin_socket const ffld_detector::expose_socket(QString const& s) const
{
	return sockets_[s];
}

void ffld_detector::start()
{
}

void ffld_detector::stop()
{
}

void ffld_detector::restart()
{
}

void ffld_detector::setupui(QWidget *parent)
{
	ui_ = new Ui_Ui();
	ui_->setupUi(parent);

	connect(ui_->browseButton, SIGNAL(clicked()),
			this, SLOT(on_browseButton_clicked()));
}

void ffld_detector::process_frame(cv::Mat const frame)
{
	if (frame.data != 0) {
		QMetaObject::invokeMethod(&detector_, "process_frame",
								Q_ARG(cv::Mat const&, frame));
	}
}

void ffld_detector::interval(QString const& v)
{
	bool ok;
	auto temp = v.toUInt(&ok);
	if (ok && temp != interval_) {
		interval_ = temp;
		ui_->intervalEdit->setText(QString::number(interval_));
		configure_detector();
	}
}

void ffld_detector::model(QString const& v)
{
	if (v != model_) {
		model_ = v;
		ui_->modelEdit->setText(model_);
		configure_detector();
	}
}

void ffld_detector::threshold(QString const& v)
{
	bool ok;
	auto temp = v.toDouble(&ok);
	if (ok && temp != threshold_) {
		threshold_ = temp;
		ui_->thresholdEdit->setText(QString::number(threshold_));
		configure_detector();
	}
}

void ffld_detector::on_browseButton_clicked()
{
	auto path = QFileDialog::getOpenFileName(0,
			tr("Select model file"), tr(""), tr(""));
	model(path);
}

void ffld_detector::on_configure_status(QString const& msg)
{
	 qDebug() << objectName() << msg;;
}

void ffld_detector::configure_detector()
{
	QMetaObject::invokeMethod(&detector_, "configure",
			Q_ARG(QString const&, model_),
			Q_ARG(unsigned const, interval_),
			Q_ARG(double const, threshold_));
}
