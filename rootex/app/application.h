#pragma once

#include "main/window.h"
#include "core/event_manager.h"
#include "os/thread.h"
#include "os/timer.h"
#include "entity_factory.h"

/// Interface for a Rootex application. 
/// Every application that uses Rootex should derive this class.
class Application
{
protected:
	ThreadPool m_ThreadPool;
	Timer m_ApplicationTimer;
	Ptr<Window> m_Window;
	
	bool initialize(const JSON::json& projectJSON);

public:
	Application();
	Application(Application&) = delete;
	virtual ~Application();

	virtual void run() = 0;
	virtual void shutDown() = 0;

	virtual String getAppTitle() const { return "Rootex Application"; }
	const Timer& getAppTimer() const { return m_ApplicationTimer; };
	const Window& getWindow() const { return *m_Window.get(); };
};

/// Externally defined function that returns a Ref object of a derived class of Application. 
/// This should be defined only once in a project. This is used by the main function to construct the Rootex application.
extern Ref<Application> CreateRootexApplication();
