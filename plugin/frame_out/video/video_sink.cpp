#include "video_sink.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

video_sink::video_sink(QString const& d) : video_sink(d)
{

}

video_sink::~video_sink()
{
	av_frame_free(&outframe_);
	avcodec_close(codecctx_);
	avformat_close_input(&fmtctx_);
}

void video_sink::start()
{
	fmt_ = av_guess_format(NULL, destination_.toStdString().c_str(), NULL);
	if (!fmt_) {
		emit error(QDateTime::currentDateTime(), "Unsupported output format");
		return;
	}
}

void video_sink::stop()
{

}
