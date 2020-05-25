#include "common/common.h"

#include "app/application.h"

#undef interface
#include "RmlUi/Core.h"

int main()
{
	Rml::Core::Box();

	Ref<Application> app = CreateRootexApplication();
	OS::Print(app->getAppTitle() + " is now starting. " + OS::GetBuildType() + " build (" + OS::GetBuildDate() + " | " + OS::GetBuildTime() + ")");
	app->run();
	app->shutDown();
	OS::Print(app->getAppTitle() + " is now safely exiting");

	return 0;
}
