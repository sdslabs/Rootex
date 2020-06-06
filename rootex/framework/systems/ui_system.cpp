#include "ui_system.h"

#include "app/application.h"
#include "core/ui/input_interface.h"

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

Rml::Core::EventListener* EventListenerInstancer::InstanceEventListener(const String& value, Rml::Core::Element* element)
{
	return new EventListener(value);
}

EventListener::EventListener(const Event::Type& eventType)
    : m_EventType(eventType)
{
}

void EventListener::ProcessEvent(Rml::Core::Event& event)
{
	EventManager::GetSingleton()->call("UIEventProxy", m_EventType, event.GetType());
}

UISystem::UISystem()
    : m_Context(nullptr)
{
	BIND_EVENT_MEMBER_FUNCTION("UISystemEnableDebugger", UISystem::enableDebugger);
	BIND_EVENT_MEMBER_FUNCTION("UISystemDisableDebugger", UISystem::disableDebugger);
}

Variant UISystem::enableDebugger(const Event* event)
{
	setDebugger(true);
	return true;
}

Variant UISystem::disableDebugger(const Event* event)
{
	setDebugger(false);
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
	
	Rml::Core::Factory::RegisterEventListenerInstancer(&s_EventListenerInstancer);

	InputInterface::Initialise();
	InputInterface::SetContext(m_Context);
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
	InputInterface::Shutdown();
	Rml::Core::RemoveContext(m_Context->GetName());
	Rml::Core::Shutdown();
}

void UISystem::setDebugger(bool enabled)
{
	Rml::Debugger::SetVisible(enabled);
}
