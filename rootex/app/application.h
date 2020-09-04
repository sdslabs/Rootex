#pragma once

#include "main/window.h"
#include "core/event_manager.h"
#include "os/timer.h"
#include "entity_factory.h"
#include "application_settings.h"

/// Interface for a Rootex application. 
/// Every application that uses Rootex should derive this class.
class Application
{
	static Application* s_Singleton;

protected:
	Timer m_ApplicationTimer;
	FrameTimer m_FrameTimer;

	Ptr<Window> m_Window;
	Ptr<ApplicationSettings> m_ApplicationSettings;
	
public:
	static Application* GetSingleton();

	Application(const String& settingsFile);
	Application(Application&) = delete;
	virtual ~Application();

	void run();
	virtual void process(float deltaMilliseconds);
	void end();

	virtual String getAppTitle() const { return "Rootex Application"; }
	const Timer& getAppTimer() const { return m_ApplicationTimer; };
	const FrameTimer& getAppFrameTimer() const { return m_FrameTimer; }
	Window* getWindow() { return m_Window.get(); };
	ApplicationSettings* getSettings() { return m_ApplicationSettings.get(); }
};

/// Externally defined function that returns a Ref object of a derived class of Application. 
/// This should be defined only once in a project. This is used by the main function to construct the Rootex application.
extern Ref<Application> CreateRootexApplication();
