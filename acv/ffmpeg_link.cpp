extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <QDebug>
#include <QMutex>


static int av_lockmgr_cb(void** mutex, enum AVLockOp op)
{
	QMutex** qmutex = (QMutex**)(mutex);
	switch (op) {
	case AV_LOCK_CREATE:
		*qmutex = new QMutex();
		break;
	case AV_LOCK_OBTAIN:
		(*qmutex)->lock();
		break;
	case AV_LOCK_RELEASE:
		(*qmutex)->unlock();
		break;
	case AV_LOCK_DESTROY:
		delete (*qmutex);
		break;
	}

	return 0;
}

void ffmpeg_link()
{
	avcodec_register_all();
	char const* sws_config = swscale_configuration();
	av_lockmgr_register(av_lockmgr_cb);
}
