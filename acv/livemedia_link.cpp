#include <Groupsock.hh>
#include <InputFile.hh>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <UsageEnvironment.hh>

void livemedia_link()
{
	Port p(0);
	CloseInputFile(0);

	TaskScheduler* sched = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*sched);
	env->reclaim();
	delete sched;
}
