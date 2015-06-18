#include "../frame_source.h"

#include <QTimer>


class rtsp_source : public frame_source {
	Q_OBJECT

	unsigned const DEFAULT_FPS = 50;

public:
	rtsp_source(QString const& s);
	~rtsp_source();

public slots:
	void start() override;
	void stop() override;

signals:
	void started() override;
	void stopped() override;
	void frame(cv::Mat const& f, double ts) override;
	void error(QString const& e) override;

private slots:
	void next();

private:
	QTimer				timer_;
	QTimer				disconnect_timer_;
};
