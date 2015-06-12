#ifndef PLUGIN_FFLD_DETECTOR_H
#define PLUGIN_FFLD_DETECTOR_H

#include "acv_plugin.h"

#include <opencv2/opencv.hpp>

#include "detector.h"


class Ui_Ui;

class ffld_detector : public acv_plugin {
	Q_OBJECT
	Q_INTERFACES(acv_plugin)
	Q_PLUGIN_METADATA(IID "ffld_detector")

public:
	ffld_detector();
	~ffld_detector();

	plugin_socket const expose_socket(QString const& s) const override;

public slots:
	void start() override;
	void stop() override;
	void restart() override;
	void setupui(QWidget *parent) override;

public slots:
	void process_frame(cv::Mat const frame);
	void interval(QString const& v);
	void model(QString const& v);
	void threshold(QString const& v);

private slots:
	void on_browseButton_clicked();
	void on_configure_status(QString const& msg);

private:
	void configure_detector();

private:
	Ui_Ui* ui_;
	QHash<QString, plugin_socket> sockets_;

	detector	detector_;
	unsigned	interval_;
	double		threshold_;
	QString		model_;
};

#endif // PLUGIN_FFLD_DETECTOR_H
