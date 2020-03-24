#include "common/common.h"

#include "app/application.h"
#include "os/thread.h"

int main()
{
	Ref<Application> app = CreateRootexApplication();
	ThreadPool m_ThreadPool;
	OS::Print(app->getAppTitle() + " is now starting. Build (" + OS::GetBuildDate() + " | " + OS::GetBuildTime() + ")");
	app->run();
	app->shutDown();
	OS::Print(app->getAppTitle() + " is now safely exiting");

	return 0;
}
