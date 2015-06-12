#ifndef PLUGIN_IMAGESHOW_H
#define PLUGIN_IMAGESHOW_H

#include "acv_plugin.h"

#include <QGraphicsScene>
#include <QImage>
#include <QPixmap>

#include <opencv2/opencv.hpp>


class Ui_ImageShowUi;

class imageshow : public acv_plugin {
	Q_OBJECT
	Q_INTERFACES(acv_plugin)
	Q_PLUGIN_METADATA(IID "imageshow")

public:
	imageshow();
	~imageshow();

	plugin_socket const expose_socket(QString const& s) const override;

public slots:
	void start() override;
	void stop() override;
	void restart() override;
	void setupui(QWidget *parent) override;

public slots:
	void process_frame(cv::Mat const frame);

private:
	QImage cvMat_to_QImage(cv::Mat const& m);
	QPixmap cvMat_to_QPixmap(cv::Mat const& m);

private:
	Ui_ImageShowUi* ui_;
	QHash<QString, plugin_socket> sockets_;
	QGraphicsScene	scene_;
};

#endif // PLUGIN_IMAGESHOW_H
