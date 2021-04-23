#pragma once

#include "main/splash_window.h"
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
	float m_DeltaMultiplier = 1.0f;
	String m_ApplicationTitle;
	int m_CurrentSaveSlot;
	JSON::json m_CurrentSaveData;

	Ptr<SplashWindow> m_SplashWindow;
	Ptr<Window> m_Window;
	Ptr<ApplicationSettings> m_ApplicationSettings;

	String getSaveSlotPath(int slot);

public:
	static Application* GetSingleton();

	Application(const String& appTitle, const String& settingsFile);
	Application(Application&) = delete;
	virtual ~Application();

	void run();
	virtual void process(float deltaMilliseconds);
	void end();

	void createSaveSlot(int slot);
	bool loadSave(int slot);
	JSON::json& getSaveData();
	bool saveSlot();

	const String& getAppTitle() const { return m_ApplicationTitle; };
	const Timer& getAppTimer() const { return m_ApplicationTimer; };
	ThreadPool& getThreadPool() { return m_ThreadPool; };
	const FrameTimer& getAppFrameTimer() const { return m_FrameTimer; }
	Window* getWindow() { return m_Window.get(); };
	ApplicationSettings* getSettings() { return m_ApplicationSettings.get(); }
	/// Returns paths of all third-party libraries provided by rootex/vendor/.
	Vector<FilePath> getLibrariesPaths();

	void destroySplashWindow();

	float* getDeltaMultiplierPtr() { return &m_DeltaMultiplier; }
	float getDeltaMultiplier() const { return m_DeltaMultiplier; }
	void setDeltaMultiplier(float gain) { m_DeltaMultiplier = gain; }
	void resetDeltaMultiplier() { setDeltaMultiplier(1.0f); }
};

/// Externally defined function that returns a Ref object of a derived class of Application.
/// This should be defined only once in a project. This is used by the main function to construct the Rootex application.
extern Ref<Application> CreateRootexApplication();
