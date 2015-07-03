#ifndef FRAME_SINK_H
#define FRAME_SINK_H

#include <QDateTime>
#include <QObject>
#include <QString>

#include <opencv2/opencv.hpp>


class frame_sink : public QObject {
public:
	frame_sink(QString const& d) : destination_(d) {}
	~frame_sink() {}

private:
	frame_sink(frame_sink const&);
	frame_sink& operator=(frame_sink const&);

public slots:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void frame(cv::Mat const& f, double ts) = 0;

signals:
	virtual void started() = 0;
	virtual void stopped() = 0;
	virtual void error(QDateTime const& ts, QString const& e) = 0;

protected:
	QString destination_;
};

#endif
