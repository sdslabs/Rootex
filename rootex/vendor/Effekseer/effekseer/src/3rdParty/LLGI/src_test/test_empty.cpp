#include "TestHelper.h"
#include "test.h"

void test_empty(LLGI::DeviceType deviceType)
{
	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("Empty", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());

	auto graphics = platform->CreateGraphics();

	while (count < 60)
	{
		if (!platform->NewFrame())
			break;

		platform->Present();
		count++;
	}

	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);
}

TestRegister Empty_Basic("Empty.Basic", [](LLGI::DeviceType device) -> void { test_empty(device); });
