#include "ui_system.h"

#include "app/application.h"

#undef interface
#include "RmlUi/Core.h"
#include "RmlUi/Debugger.h"
#include "RmlUi/Controls/Controls.h"
#define interface __STRUCT__

double CustomSystemInterface::GetElapsedTime()
{
	return Application::GetSingleton()->getAppTimer().getTimeMs() * MS_TO_S;
}

bool CustomSystemInterface::LogMessage(Rml::Core::Log::Type type, const String& message)
{
	switch (type)
	{
	case Rml::Core::Log::LT_ERROR:
		ERR("RmlUi Error: " + message);
		return false;
		break;
	case Rml::Core::Log::LT_ASSERT:
		PRINT("RmlUi Assert: " + message)
		return false;
		break;
	case Rml::Core::Log::LT_WARNING:
		WARN("RmlUi Warn: " + message)
		break;
	case Rml::Core::Log::LT_INFO:
		PRINT("RmlUi Info: " + message)
		break;
	default:
		break;
	}

	return true;
}

UISystem* UISystem::GetSingleton()
{
	static UISystem singleton;
	return &singleton;
}

void UISystem::loadFont(const String& path)
{
	if (!Rml::Core::LoadFontFace(path))
	{
		ERR("Could not load font through RmlUi: " + path);
	}
}

Rml::Core::ElementDocument* UISystem::loadDocument(const String& path)
{
	Rml::Core::ElementDocument* document = m_Context->LoadDocument(path);
	if (document)
	{
		document->Show();
	}
	else
	{
		WARN("Could not load document: " + path);
	}

	return document;
}

void UISystem::unloadDocument(Rml::Core::ElementDocument* document)
{
	m_Context->UnloadDocument(document);
}

void UISystem::initialize(int width, int height)
{
	m_RmlSystemInterface.reset(new CustomSystemInterface());
	m_RmlRenderInterface.reset(new CustomRenderInterface(width, height));

	Rml::Core::SetSystemInterface(m_RmlSystemInterface.get());
	Rml::Core::SetRenderInterface(m_RmlRenderInterface.get());
	if (!Rml::Core::Initialise())
	{
		ERR("Could not initialize RmlUi UI rendering library");
	}
	Rml::Controls::Initialise();
	loadFont("rootex/assets/fonts/Lato-Regular.ttf");
	m_Context = Rml::Core::CreateContext("default", Rml::Core::Vector2i(width, height));
	Rml::Debugger::Initialise(m_Context);
	Rml::Debugger::SetVisible(true);
}

void UISystem::update()
{
	m_Context->Update();
}

void UISystem::render()
{
	RenderingDevice::GetSingleton()->setAlphaBlendState();
	RenderingDevice::GetSingleton()->setTemporaryUIRasterizerState();
	m_Context->Render();
}

void UISystem::shutdown()
{
	Rml::Core::RemoveContext(m_Context->GetName());
	Rml::Core::Shutdown();
}
