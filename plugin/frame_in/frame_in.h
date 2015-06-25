#ifndef FRAME_IN_H
#define FRAME_IN_H

#include "acv_plugin.h"

#include <opencv2/opencv.hpp>

#include <QDateTime>
#ifdef WITH_GUI
#include <QGraphicsScene>
#include <QImage>
#include <QPixmap>
#endif

class QThread;
class QTimer;
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
	plugin_options const& options() const override;
	void init() override;

public slots:
	// Plugin options
	void source(QString const& v);
	void recover_on_error(bool v);
	void recover_interval(double v);
	void read_timeout(double v);
	void start();
	void stop();

signals:
	// Player status
	void started();
	void stopped();
	void frame(cv::Mat const& frame, double ts);
	void error(QDateTime const& ts, QString e);

private slots:
	// Internal handlers for frame source events
	void on_source_started();
	void on_source_stopped();
	void on_source_frame(cv::Mat const& frame, double ts);
	void on_source_error(QDateTime const& ts, QString e);
	void on_source_read_timeout();

private:
	void connect_to_source(frame_source* s);

private:
	QHash<QString, plugin_socket>	sockets_;
	plugin_options	options_;
	QString			source_;
	bool			recover_			= false;
	double			recover_interval_	= 0;
	double			read_timeout_		= 0;

	QThread*		workthread_		= nullptr;
	frame_source*	fs_				= nullptr;
	QTimer*			read_deadline_	= nullptr;
	QTimer*			recover_timer_	= nullptr;

#ifdef WITH_GUI
public slots:
	void setupui(QWidget* parent) override;

private slots:
	void liveview(cv::Mat const& f, double ts);
private:
	QImage	cvMat_to_QImage(cv::Mat const& m);
	QPixmap	cvMat_to_QPixmap(cv::Mat const& m);
	Ui_FrameInUi*		ui_ = 0;
	QGraphicsScene		scene_;
#endif

};


#endif
