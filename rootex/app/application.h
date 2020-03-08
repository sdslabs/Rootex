#pragma once

#include "main/window.h"
#include "core/event_manager.h"
#include "os/thread.h"
#include "os/timer.h"
#include "entity_factory.h"

class Application
{
	bool initialize(String windowConfigFilePath);

protected:
	ThreadPool m_ThreadPool;
	Timer m_ApplicationTimer;
	Ptr<Window> m_Window;

public:
	Application(const String& windowConfigFilePath);
	Application(Application&) = delete;
	virtual ~Application();
	
	Ref<Entity> addEntity(String path);

	virtual void run() = 0;
	virtual void shutDown() = 0;

	virtual String getAppTitle() const { return "Rootex Application"; }
	const Timer& getAppTimer() const { return m_ApplicationTimer; };
	const Window& getWindow() const { return *m_Window.get(); };
};

extern Ref<Application> CreateRootexApplication();
