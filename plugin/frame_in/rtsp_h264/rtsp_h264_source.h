#include "../frame_source.h"

#include <QTime>


class QTimer;

class TaskScheduler;
class UsageEnvironemt;


class rtsp_h264_source : public frame_source {
	Q_OBJECT

	unsigned const DEFAULT_FPS = 50;

public:
	rtsp_h264_source(QString const& s);
	~rtsp_h264_source();

public slots:
	void start() override;
	void stop() override;

signals:
	void started() override;
	void stopped() override;
	void frame(cv::Mat const& f, double ts) override;
	void error(QDateTime const& ts, QString const& e) override;

private slots:
	void next();

private:
	QTimer*				timer_;
	QTime				start_ts_;
};
