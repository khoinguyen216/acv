#include "rtsp_source.h"

#include <cassert>

#include <QDebug>
#include <QTimer>

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <UsageEnvironment.hh>

#include "rtsp_handlers.h"
#include "sched_thread.h"
#include "MPRTSPClient.h"

rtsp_source::rtsp_source(QString const& s) : frame_source(s),
	timer_(new QTimer(this))
{
	assert(!s.isEmpty());


	sched_thread_ = new sched_thread();
}

rtsp_source::~rtsp_source()
{
	delete sched_;
}

void rtsp_source::start()
{
	if (client_ != nullptr)
		return;

	sched_	= BasicTaskScheduler::createNew();
	env_	= BasicUsageEnvironment::createNew(*sched_);

	sched_thread_->setScheduler(sched_);
	sched_thread_->enableScheduler();
	sched_thread_->start();
	client_ = MPRTSPClient::createNew(*env_, source_.toStdString().c_str(),
				this, 2, "kclient");
	if (client_ == nullptr) {
		emit error(QDateTime::currentDateTime(),
				QString("Could not create client for %1").arg(source_));
		return;
	}

	client_->sendDescribeCommand(after_describe);
	start_ts_.start();
	emit started();
}

void rtsp_source::stop()
{
	sched_thread_->disableScheduler();
	sched_thread_->wait();
	shutdown(client_);
	Medium::close(client_); client_ = nullptr;
	env_->reclaim();
	delete sched_; sched_ = nullptr;
	emit stopped();
}
