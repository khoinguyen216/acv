#ifndef PLUGIN_IMAGEFEED_H
#define PLUGIN_IMAGEFEED_H

#include "acv_plugin.h"

#include <opencv2/opencv.hpp>


class QDirIterator;
class QTimer;
class Ui_ImageFeedUi;

class imagefeed : public acv_plugin {
	Q_OBJECT
	Q_INTERFACES(acv_plugin)
	Q_PLUGIN_METADATA(IID "imagefeed")

public:
	imagefeed();
	~imagefeed();

	plugin_socket const expose_socket(QString const& s) const override;

public slots:
	void start() override;
	void stop() override;
	void restart() override;
	void setupui(QWidget *parent) override;

public slots:
	void src(QString const& v);
	void loop(QString const& v);
	void autostart(QString const& v);

signals:
	void frame_ready(cv::Mat const frame);

public slots:
	void next_image();
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void on_pathEdit_textChanged(QString const& t);

private:
	Ui_ImageFeedUi*	ui_;
	QTimer*			timer_;
	QDirIterator*	dirit_;
	QString			src_;
	bool			loop_;
	bool			autostart_;
	QHash<QString, plugin_socket> sockets_;
};

#endif
