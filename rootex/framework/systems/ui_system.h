#pragma once

#include "system.h"
#include "event_manager.h"
#include "core/ui/custom_render_interface.h"

#undef interface
#include "RmlUi/Core/SystemInterface.h"
#define interface __STRUCT__

class CustomSystemInterface : public Rml::Core::SystemInterface
{
	virtual double GetElapsedTime() override;
	virtual bool LogMessage(Rml::Core::Log::Type type, const String& message) override;
};

class EventListener : public Rml::Core::EventListener
{
	Event::Type m_EventType;

public:
	EventListener(const Event::Type& eventType);

	virtual void ProcessEvent(Rml::Core::Event& event) override;
};

class EventListenerInstancer : public Rml::Core::EventListenerInstancer
{
public:
	virtual Rml::Core::EventListener* InstanceEventListener(const String& value, Rml::Core::Element* element) override;
};

class UISystem : public System
{
	static inline EventListenerInstancer s_EventListenerInstancer;

	Ptr<CustomSystemInterface> m_RmlSystemInterface;
	Ptr<CustomRenderInterface> m_RmlRenderInterface;
	Rml::Core::Context* m_Context;

	UISystem();
	virtual ~UISystem() = default;
	
	Variant enableDebugger(const Event* event);
	Variant disableDebugger(const Event* event);

public:
	static UISystem* GetSingleton();

	void loadFont(const String& path);
	Rml::Core::ElementDocument* loadDocument(const String& path);

	bool initialize(const JSON::json& systemData) override;
	void update(float deltaMilliseconds) override;
	void end() override;

	void setDebugger(bool enabled);
};
