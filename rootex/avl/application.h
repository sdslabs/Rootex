#pragma once

#include "core/resource_loader.h"
#include "os/thread.h"
#include "os/timer.h"
#include "main/window.h"

class Application
{
	ThreadPool m_ThreadPool;
	Timer m_ApplicationTime;
	Ptr<Window> m_Window;

public:
	Application(const String& windowConfigFilePath);
	Application(Application&) = delete;
	~Application();

	virtual bool initialize(String windowConfigFilePath);
	virtual void createGameAndView() = 0;
	virtual void shutdown();
};

extern Ref<Application> CreateRootexApplication();
