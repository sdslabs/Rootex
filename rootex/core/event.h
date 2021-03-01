#pragma once

#include "common/common.h"
#include "entity.h"

#define DEFINE_EVENT(eventName, ...) const static inline Event::Type eventName = #eventName

/// An Event that is sent out by EventManager.
class Event
{
public:
	/// String defining the type of the event.
	typedef String Type;

private:
	Type m_Type;
	Variant m_Data;

public:
	Event(const Type type, const Variant data);
	Event(Event&) = delete;
	~Event() = default;

	const Type& getType() const { return m_Type; };
	/// Returns the payload data sent with an event. Extract typed data after getting the data.
	const Variant& getData() const { return m_Data; }
};

struct RootexEvents
{
	/// Application exited the main loop
	DEFINE_EVENT(ApplicationExit);

	/// Delete the Scene* passed in
	DEFINE_EVENT(DeleteScene, Scene*);

	/// SceneLoader opened a new Scene
	DEFINE_EVENT(OpenedScene);

	/// A string was printed to output
	DEFINE_EVENT(OSPrint, String);

	/// UISystem debugger on/off
	DEFINE_EVENT(UISystemEnableDebugger);

	/// UISystem debugger on/off
	DEFINE_EVENT(UISystemDisableDebugger);

	/// Get bool if window is fullscreen
	DEFINE_EVENT(WindowGetScreenState);

	/// Window has resized to size passed in
	DEFINE_EVENT(WindowResized, Vector2);

	/// Toggle the application window fullscreen mode
	DEFINE_EVENT(WindowToggleFullscreen);

	/// Window has requested to quit
	DEFINE_EVENT(QuitWindowRequest);

	/// Editor has requested to quit
	DEFINE_EVENT(QuitEditorWindow);
};
