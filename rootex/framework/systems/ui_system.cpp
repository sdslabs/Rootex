#include "ui_system.h"

#include "app/application.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

double CustomSystemInterface::GetElapsedTime()
{
	return Application::GetSingleton()->getAppTimer().getTimeMs() * MS_TO_S;
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
		ERR("Could not load font through RmlUi");
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
	Rml::Core::SetSystemInterface(&m_RmlSystemInterface);
	Rml::Core::SetRenderInterface(&m_RmlRenderInterface);
	if (!Rml::Core::Initialise())
	{
		ERR("Could not initialize RmlUi UI renderering library");
	}
	loadFont("rootex/assets/fonts/Lato-Regular.ttf");
	m_Context = Rml::Core::CreateContext("default", Rml::Core::Vector2i(width, height));
}

void UISystem::update()
{
	m_Context->Update();
}

void UISystem::render()
{
	m_Context->Render();
}

void UISystem::shutdown()
{
	Rml::Core::RemoveContext(m_Context->GetName());
	Rml::Core::Shutdown();
}
