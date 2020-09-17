#include "ui_system.h"

#include "app/application.h"
#include "core/ui/input_interface.h"

#undef interface
#include "RmlUi/Core.h"
#include "RmlUi/Debugger.h"
#include "RmlUi/Lua.h"
#define interface __STRUCT__

double CustomSystemInterface::GetElapsedTime()
{
	return Application::GetSingleton()->getAppTimer().getTimeMs() * MS_TO_S;
}

bool CustomSystemInterface::LogMessage(Rml::Log::Type type, const String& message)
{
	switch (type)
	{
	case Rml::Log::LT_ERROR:
		ERR("RmlUi Error: " + message);
		return false;
		break;
	case Rml::Log::LT_ASSERT:
		PRINT("RmlUi Assert: " + message)
		return false;
		break;
	case Rml::Log::LT_WARNING:
		WARN("RmlUi Warn: " + message)
		break;
	case Rml::Log::LT_INFO:
		PRINT("RmlUi Info: " + message)
		break;
	default:
		break;
	}

	return true;
}

UISystem::UISystem()
    : System("UISystem", UpdateOrder::UI, true)
	, m_Context(nullptr)
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
	if (!Rml::LoadFontFace(path))
	{
		ERR("Could not load font through RmlUi: " + path);
	}
}

Rml::ElementDocument* UISystem::loadDocument(const String& path)
{
	Rml::ElementDocument* document = m_Context->LoadDocument(path);
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

bool UISystem::initialize(const JSON::json& systemData)
{
	m_RmlSystemInterface.reset(new CustomSystemInterface());
	m_RmlRenderInterface.reset(new CustomRenderInterface(systemData["width"], systemData["height"]));

	Rml::SetSystemInterface(m_RmlSystemInterface.get());
	Rml::SetRenderInterface(m_RmlRenderInterface.get());

	if (!Rml::Initialise())
	{
		ERR("Could not initialize RmlUi UI rendering library");
		return false;
	}

	Rml::Lua::Initialise(LuaInterpreter::GetSingleton()->getLuaState().lua_state());
	
	loadFont("rootex/assets/fonts/Lato-Regular.ttf");
	
	m_Context = Rml::CreateContext("default", Rml::Vector2i(systemData["width"], systemData["height"]));
	
	Rml::Debugger::Initialise(m_Context);
	
	InputInterface::Initialise();
	InputInterface::SetContext(m_Context);

	return true;
}

void UISystem::update(float deltaMilliseconds)
{
	m_Context->Update();
	RenderingDevice::GetSingleton()->setAlphaBlendState();
	RenderingDevice::GetSingleton()->setTemporaryUIRasterizerState();
	m_Context->Render();
}

void UISystem::shutDown()
{
	Rml::Shutdown();
}

void UISystem::setDebugger(bool enabled)
{
	Rml::Debugger::SetVisible(enabled);
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void UISystem::draw()
{
	System::draw();
	
	static bool debugger = false;
	if (ImGui::Checkbox("Debugger", &debugger))
	{
		setDebugger(debugger);
	}

	ImGui::Text("Press F8 in game");
}
#endif
