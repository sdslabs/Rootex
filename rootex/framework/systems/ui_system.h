#pragma once

#include "system.h"
#include "core/ui/custom_render_interface.h"

#undef interface
#include "RmlUi/Core/SystemInterface.h"
#define interface __STRUCT__

class CustomSystemInterface : public Rml::Core::SystemInterface
{
	virtual double GetElapsedTime() override;
	virtual bool LogMessage(Rml::Core::Log::Type type, const String& message) override;
};

class UISystem : public System
{
	Ptr<CustomSystemInterface> m_RmlSystemInterface;
	Ptr<CustomRenderInterface> m_RmlRenderInterface;
	Rml::Core::Context* m_Context;

public:
	static UISystem* GetSingleton();

	void loadFont(const String& path);
	Rml::Core::ElementDocument* loadDocument(const String& path);
	void unloadDocument(Rml::Core::ElementDocument* document);

	void initialize(int width, int height);
	void update();
	void render();
	void shutdown();
};
