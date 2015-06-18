#include "FFmpegCapture.hpp"

#include <stdexcept>

boost::posix_time::ptime epoch;


double CLOCK()
{
    static bool _clockInited = false;
	if (!_clockInited) {
		epoch = boost::posix_time::microsec_clock::local_time();
		_clockInited = true;
	}
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration diff = now - epoch;// time_t_epoch;
	double x = diff.total_milliseconds();
	return x;
}

cv::Mat avframe_to_cvmat(AVFrame *picrgb, AVCodecContext* ccontext)
{

	int     nChannels;
	int     stepWidth;
	uchar*  pData;
	cv::Mat frameImage(cv::Size(ccontext->width, ccontext->height), CV_8UC3, cv::Scalar(0));
	stepWidth = frameImage.step;
	nChannels = frameImage.channels();
	pData = frameImage.data;

	for (int i = 0; i < ccontext->height; i++)
	{
		for (int j = 0; j < ccontext->width; j++)
		{
			pData[i*stepWidth + j*nChannels + 0] = picrgb->data[0][i*picrgb->linesize[0] + j*nChannels + 2];
			pData[i*stepWidth + j*nChannels + 1] = picrgb->data[0][i*picrgb->linesize[0] + j*nChannels + 1];
			pData[i*stepWidth + j*nChannels + 2] = picrgb->data[0][i*picrgb->linesize[0] + j*nChannels + 0];
		}
	}

	return frameImage;
}
double FFmpegCapture::lastframeseen = 0;
bool FFmpegCapture::haserror = true;
FFmpegCapture::FFmpegCapture()
{
	mInited = false;
	mUrl = "";

	pic = NULL;
	picrgb = NULL;
	picture_buf = NULL;
	picture_buf2 = NULL;
	context = NULL;
	oc = NULL;
	opts = NULL;

	isrunning = true;
}
FFmpegCapture::FFmpegCapture(string url,AVCodecID codec)
{
	mInited = false;
	mUrl = url;
	mCodec = codec;

	pic = NULL;
	picrgb = NULL;
	picture_buf = NULL;
	picture_buf2 = NULL;
	context = NULL;
	oc = NULL;
	opts = NULL;
	isrunning = true;
}
int  FFmpegCapture::ffmpeg_callback(void* ctx)
{
	if (CLOCK() - lastframeseen > 5000)
	{
		lastframeseen = CLOCK();
		//throw std::runtime_error("time out");
		return 1;
	}
	//cout << "error here" << endl;
	return 0;
}
void FFmpegCapture::log_default_callback(void *      avcl,
	int     level,
	const char *    fmt,
	va_list     vl
	)
{
	if (level > 10)
	{
		//cout << level << endl;
		haserror = true;
	}
	av_log_default_callback(avcl,level,fmt,vl);
	return;
}

void FFmpegCapture::DecodeWorker()
{
	try
	{

		context = avformat_alloc_context();
		const AVIOInterruptCB int_cb = { this->ffmpeg_callback, NULL };
		context->interrupt_callback = int_cb;
		av_log_set_callback(this->log_default_callback);
		av_register_all();
		avformat_network_init();
		//av_log_set_level(AV_LOG_PANIC);
		if (avformat_open_input(&context, mUrl.c_str(), NULL, NULL) != 0){
			throw std::runtime_error("failed to open stream");
		}

		if (avformat_find_stream_info(context, NULL) < 0){
			throw std::runtime_error("failed to open stream");
		}

		//search video stream
		for (int i = 0; i<context->nb_streams; i++){
			if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
				video_stream_index = i;
		}

		av_init_packet(&packet);

		//open output file
		//AVOutputFormat* fmt = av_guess_format(NULL,"test2.mp4",NULL);
		oc = avformat_alloc_context();
		//oc->oformat = fmt;
		//avio_open2(&oc->pb, "test.mp4", AVIO_FLAG_WRITE,NULL,NULL);

		stream = NULL;
		int cnt = 0;
		//start reading packets from stream and write them to file
		av_read_play(context);//play RTSP

		codec = NULL;
		codec = avcodec_find_decoder(mCodec);
		if (!codec)
		{
			throw std::runtime_error("codec not found");
		}

		ccontext = avcodec_alloc_context3(codec);
		avcodec_get_context_defaults3(ccontext, codec);
		avcodec_copy_context(ccontext, context->streams[video_stream_index]->codec);

		ccontext->err_recognition |= AV_EF_EXPLODE;

		context->streams[video_stream_index]->discard = AVDISCARD_DEFAULT;
		opts = 0;
		/*
		int ret = av_dict_set(&opts, "rtsp_transport", "tcp", 0);
		av_dict_set(&opts, "discardcorruput", "1", 0);

		av_dict_set(&opts, "bufsize", "30000000", 0);
		av_dict_set(&opts, "er", "0", 0);
		ccontext->error_concealment = 0;*/
		//av_dict_set(&opts, "discardcorrupt", "1", 0);
		//ret = av_dict_set(&opts, "timeout", "300",0);
		//int ret = av_dict_set(&opts, "circular", "tcp", 0);


		if (avcodec_open2(ccontext, codec, &opts) < 0)
		{
			throw std::runtime_error("error opening codec");
		}

		img_convert_ctx = sws_getContext(ccontext->width, ccontext->height, ccontext->pix_fmt, ccontext->width, ccontext->height,
			PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

		int size = avpicture_get_size(PIX_FMT_YUV420P, ccontext->width, ccontext->height);
		picture_buf = (uint8_t*)(av_malloc(size));
		pic = avcodec_alloc_frame();
		picrgb = avcodec_alloc_frame();
		int size2 = avpicture_get_size(PIX_FMT_RGB24, ccontext->width, ccontext->height);
		picture_buf2 = (uint8_t*)(av_malloc(size2));
		avpicture_fill((AVPicture *)pic, picture_buf, PIX_FMT_YUV420P, ccontext->width, ccontext->height);
		avpicture_fill((AVPicture *)picrgb, picture_buf2, PIX_FMT_RGB24, ccontext->width, ccontext->height);


		while (av_read_frame(context, &packet) >= 0)
		{
			if (packet.stream_index == video_stream_index){//packet is video
				if (stream == NULL)
				{//create stream in file
					stream = avformat_new_stream(oc, context->streams[video_stream_index]->codec->codec);
					avcodec_copy_context(stream->codec, context->streams[video_stream_index]->codec);
					stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio;
				}
				int check = 0;
				packet.stream_index = stream->id;
				haserror = false;

				decodeMtx.lock();
				int result = avcodec_decode_video2(ccontext, pic, &check, &packet);
				isimready = true;
				decodeMtx.unlock();


				av_free_packet(&packet);
				av_init_packet(&packet);

			}
		}

	}

	catch (...)
	{

	}

	isrunning = false;
}
void FFmpegCapture::Init()
{
	try
	{

		lastframeseen = CLOCK();
		mInited = true;
		isimready = false;
		decodeThread = new boost::thread(boost::bind(&FFmpegCapture::DecodeWorker, this));
	}
	catch (const std::runtime_error& ex) {
		throw ex;
	}
}
FFmpegCapture& FFmpegCapture::operator >>(cv::Mat& out)
{
	if (!mInited)
	{
		Init();
	}

	while (isrunning)
	{
		if (!haserror && isimready)
		{

			decodeMtx.lock();
			sws_scale(img_convert_ctx, pic->data, pic->linesize, 0, ccontext->height, picrgb->data, picrgb->linesize);
			isimready = false;
			decodeMtx.unlock();
			Mat im = avframe_to_cvmat(picrgb, ccontext);
			lastframeseen = CLOCK();
			out = im.clone();
			return *this;
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(20));
	}

	throw std::runtime_error("ffmpegcapture: Unable to continue decoding because of errors.");
}
FFmpegCapture::~FFmpegCapture()
{
	if (mInited)
	{

		if(pic) av_free(pic);
		if (picrgb) av_free(picrgb);
		if (picture_buf)av_free(picture_buf);
		if (picture_buf2)av_free(picture_buf2);

		if (context)av_read_pause(context);
		if (oc)avio_close(oc->pb);
		if (oc)avformat_free_context(oc);

		if (opts)av_dict_free(&opts);
	}

}
