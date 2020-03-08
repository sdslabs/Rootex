#include "common/common.h"

#include "app/application.h" 

#define MAX_STACK_TRACE_DEPTH 32

int main()
{
	try
	{
		Ref<Application> app = CreateRootexApplication();
		OS::Print(app->getAppTitle() + " is now starting. Build (" + OS::GetBuildDate() + " | " + OS::GetBuildTime() + ")");
		app->run();
		app->shutDown();
		OS::Print(app->getAppTitle() + " is now safely exiting");
	}
	catch (std::exception& e)
	{
		ERR(String("Exception: ") + e.what());
		OS::Print("Rootex has crashed");
	}

	return 0;
}
