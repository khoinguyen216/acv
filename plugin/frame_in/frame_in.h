#ifndef FRAME_IN_H
#define FRAME_IN_H

#include "acv_plugin.h"

#include <opencv2/opencv.hpp>
#ifdef WITH_GUI
#include <QGraphicsScene>
#include <QImage>
#include <QPixmap>
#endif


#ifdef WITH_GUI
class Ui_FrameInUi;
#endif
class frame_source;

class frame_in : public acv_plugin {
	Q_OBJECT
	Q_INTERFACES(acv_plugin)
	Q_PLUGIN_METADATA(IID "frame_in")

	// Default options
	unsigned const	RETY_INTERVAL	= 1; // in seconds
	unsigned const	SKIP_FRAMES		= 0;

public:
	frame_in();
	~frame_in();
	plugin_socket const expose_socket(QString const& s) const override;
	void init() override;

public slots:
	void source(QString const& v);

signals:
	// Player controls
	void start();
	void stop();

signals:
	// Player status
	void started();
	void stopped();
	void frame(cv::Mat const& frame, double ts);

private:
	void connect_to_source(frame_source* s);

private:
	QHash<QString, plugin_socket>	sockets_;
	QString			source_;
	frame_source*	fs_ = 0;

#ifdef WITH_GUI
public slots:
	void setupui(QWidget* parent) override;

private slots:
	void liveview(cv::Mat const& f, double ts);
	void on_started();
	void on_stopped();

private:
	QImage	cvMat_to_QImage(cv::Mat const& m);
	QPixmap	cvMat_to_QPixmap(cv::Mat const& m);
	Ui_FrameInUi*		ui_ = 0;
	QGraphicsScene		scene_;
#endif

};


#endif
