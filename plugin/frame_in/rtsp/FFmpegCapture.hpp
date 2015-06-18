#ifndef FFMPEGCAPTURE_HPP
#define FFMPEGCAPTURE_HPP

#include <stdint.h>
extern "C" {
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"

}
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

double CLOCK();
cv::Mat avframe_to_cvmat(AVFrame *picrgb, AVCodecContext* ccontext);
class FFmpegCapture
{
public:
	FFmpegCapture();//copy constructor
	FFmpegCapture(string url, AVCodecID codec = CODEC_ID_H264);
	~FFmpegCapture();
	FFmpegCapture& operator >>(cv::Mat& out);

	boost::mutex decodeMtx;
	boost::thread* decodeThread;
	void DecodeWorker();
	static void log_default_callback(void *      avcl,
		int     level,
		const char *    fmt,
		va_list     vl
		);
	static int ffmpeg_callback(void* ctx);
	static double lastframeseen;
	static bool haserror;
	bool isrunning;
private:
	void Init();
	bool mInited;
	bool isimready;
	string mUrl;
	AVCodecID mCodec;


	AVPacket packet;
	SwsContext *img_convert_ctx;
	AVFormatContext* context;
	int video_stream_index;
	AVFormatContext* oc;
	AVStream* stream;
	AVCodec *codec;
	AVCodecContext* ccontext;
	AVDictionary *opts;
	uint8_t* picture_buf;
	AVFrame* pic;
	AVFrame* picrgb;
	uint8_t* picture_buf2;
};
#endif
