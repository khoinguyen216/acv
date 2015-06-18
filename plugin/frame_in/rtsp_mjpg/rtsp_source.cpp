#include "rtsp_source.h"

#include <cassert>

#include <QDebug>


rtsp_source::rtsp_source(QString const& s) : frame_source(s)
{
	assert(!s.isEmpty());

	connect(&timer_, SIGNAL(timeout()), this, SLOT(next()));
}

rtsp_source::~rtsp_source()
{
}

void rtsp_source::start()
{
}

void rtsp_source::stop()
{
}

void rtsp_source::next()
{
}
