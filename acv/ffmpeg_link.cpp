extern "C" {
#include <libavcodec/avcodec.h>
}


void ffmpeg_link()
{
	avcodec_register_all();
}
