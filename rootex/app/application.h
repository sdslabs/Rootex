#pragma once

#include "main/window.h"
#include "core/event_manager.h"
#include "os/timer.h"
#include "os/thread.h"
#include "application_settings.h"

/// Interface for a Rootex application.
/// Every application that uses Rootex should derive this class.
class Application
{
	static Application* s_Singleton;

protected:
	Timer m_ApplicationTimer;
	FrameTimer m_FrameTimer;
	ThreadPool m_ThreadPool;
	float m_DeltaGain = 0.0f;

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
	ThreadPool& getThreadPool() { return m_ThreadPool; };
	const FrameTimer& getAppFrameTimer() const { return m_FrameTimer; }
	Window* getWindow() { return m_Window.get(); };
	ApplicationSettings* getSettings() { return m_ApplicationSettings.get(); }
	/// Returns paths of all third-party libraries provided by rootex/vendor/.
	Vector<FilePath> getLibrariesPaths();

	float* getDeltaGainPtr() { return &m_DeltaGain; }
	float getDeltaGain() const { return m_DeltaGain; }
	void setDeltaGain(float gain) { m_DeltaGain = gain; }
	void resetDeltaGain() { setDeltaGain(0.0f); }
};

/// Externally defined function that returns a Ref object of a derived class of Application.
/// This should be defined only once in a project. This is used by the main function to construct the Rootex application.
extern Ref<Application> CreateRootexApplication();
