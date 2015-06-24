extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}


void ffmpeg_link()
{
	avcodec_register_all();
	char const* sws_config = swscale_configuration();
}
