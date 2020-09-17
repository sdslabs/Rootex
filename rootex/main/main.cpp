#include "common/common.h"

#include "app/application.h"

int main()
{
	Ref<Application> app = CreateRootexApplication();
	OS::Print(app->getAppTitle() + " is now starting. " + OS::GetBuildType() + " build (" + OS::GetBuildDate() + " | " + OS::GetBuildTime() + ")");
	app->run();
	app->end();
	OS::Print(app->getAppTitle() + " is now safely exiting");

	return 0;
}
