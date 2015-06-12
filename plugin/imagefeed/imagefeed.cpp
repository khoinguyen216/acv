#include "imagefeed.h"
#include "ui_imagefeed.h"

#include <cassert>

#include <QDebug>
#include <QDirIterator>
#include <QTimer>

#include <opencv2/opencv.hpp>


using cv::Mat;
using cv::imread;

imagefeed::imagefeed()
{
	// Set up socket interfaces
	sockets_["video"].dir_out = true;
	sockets_["video"].wires["frame"] = "frame_ready(cv::Mat const)";
}

imagefeed::~imagefeed()
{
}

plugin_socket const imagefeed::expose_socket(QString const& s) const
{
	return sockets_[s];
}

void imagefeed::start()
{
	if (!autostart_) {
		autostart_ = true;
		return;
	}

	dirit_ = new QDirIterator(src_);
	timer_ = new QTimer(this);
	connect(timer_, SIGNAL(timeout()), this, SLOT(next_image()));
	timer_->start(1000);
	ui_->startButton->setEnabled(false);
	ui_->stopButton->setEnabled(true);
	ui_->pathEdit->setEnabled(false);
}

void imagefeed::stop()
{
	delete timer_;
	ui_->startButton->setEnabled(true);
	ui_->stopButton->setEnabled(false);
	ui_->pathEdit->setEnabled(true);
}

void imagefeed::restart()
{
}

void imagefeed::setupui(QWidget *parent)
{
	ui_ = new Ui_ImageFeedUi();
	ui_->setupUi(parent);

	connect(ui_->startButton, SIGNAL(clicked()),
			this, SLOT(on_startButton_clicked()));
	connect(ui_->stopButton, SIGNAL(clicked()),
			this, SLOT(on_stopButton_clicked()));
	connect(ui_->pathEdit, SIGNAL(textChanged(QString)),
			this, SLOT(on_pathEdit_textChanged(QString const&)));
}

void imagefeed::src(QString const& v)
{
	if (src_ != v) {
		src_ = v;
		ui_->pathEdit->setText(src_);
	}
}

void imagefeed::loop(QString const& v)
{
	if (v == "yes")
		loop_ = true;
	else if (v == "no")
		loop_ = false;
}

void imagefeed::autostart(QString const& v)
{
	if (v == "yes")
		autostart_ = true;
	else if (v == "no")
		autostart_ = false;
}

void imagefeed::next_image()
{
	if (dirit_ != 0 && dirit_->hasNext()) {
		auto const& cpath = dirit_->next().toStdString();
		if (dirit_->fileName() != "." && dirit_->fileName() != "..") {
			auto im = imread(cpath);
			emit frame_ready(im);
		}
	} else if (loop_) {
		delete dirit_;
		dirit_ = new QDirIterator(src_);
		next_image();
	} else
		stop();
}

void imagefeed::on_startButton_clicked()
{
	start();
}

void imagefeed::on_stopButton_clicked()
{
	stop();
}

void imagefeed::on_pathEdit_textChanged(QString const& t)
{
	if (t != src_)
		src(t);
}
