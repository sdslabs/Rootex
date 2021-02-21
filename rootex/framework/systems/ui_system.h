#pragma once

#include "system.h"
#include "event_manager.h"
#include "core/ui/custom_render_interface.h"

#undef interface
#include "RmlUi/Core/SystemInterface.h"
#define interface __STRUCT__

class CustomSystemInterface : public Rml::SystemInterface
{
	virtual double GetElapsedTime() override;
	virtual bool LogMessage(Rml::Log::Type type, const String& message) override;
};

class UISystem : public System
{
	Ptr<CustomSystemInterface> m_RmlSystemInterface;
	Ptr<CustomRenderInterface> m_RmlRenderInterface;
	Rml::Context* m_Context;

	UISystem();
	virtual ~UISystem() = default;

	Variant enableDebugger(const Event* event);
	Variant disableDebugger(const Event* event);

public:
	static UISystem* GetSingleton();

	void loadFont(const String& path);
	Rml::ElementDocument* loadDocument(const String& path);
	void unloadDocument(Rml::ElementDocument*& document);

	bool initialize(const JSON::json& systemData) override;
	void update(float deltaMilliseconds) override;
	void shutDown();

	void setDebugger(bool enabled);
	Rml::Context* getContext() { return m_Context; }

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
