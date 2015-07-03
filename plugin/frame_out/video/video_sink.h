#ifndef VIDEO_SINK_H
#define VIDEO_SINK_H

#include "frame_sink.h"

#include <QString>


class AVCodec;
class AVCodecContext;
class AVFormatContext;
class AVOutputFormat;
class AVStream;
class AVFrame;

class video_sink : public frame_sink {
	Q_OBJECT

	unsigned const	DEFAULT_FPS		= 25;
	unsigned const	DEFAULT_XRES	= 640;
	unsigned const	DEFAULT_YRES	= 480;
	unsigned const	DEFAULT_BITRATE	= 400000;

public:
	video_sink(QString const& d);
	~video_sink();

public slots:
	void start() override;
	void stop() override;

signals:
	void started() override;
	void stopped() override;
	void frame(cv::Mat const& f, double ts) override;
	void error(QDateTime const& ts, QString const& e) override;

private:
	QString				destination_;
	AVCodec*			codec_		= NULL;
	AVCodecContext*		codecctx_	= NULL;
	AVOutputFormat*		fmt_		= NULL;
	AVFormatContext*	fmtctx_		= NULL;
	AVStream*			vst_		= NULL;
	AVFrame*			outframe_	= NULL;
	unsigned			xres_		= DEFAULT_XRES;
	unsigned			yres_		= DEFAULT_YRES;
	unsigned			bitrate_	= DEFAULT_BITRATE;
};

#endif
