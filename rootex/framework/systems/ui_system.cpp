#include "ui_system.h"

#include "app/application.h"
#include "core/ui/input_interface.h"
#include "core/ui/rootex_decorator.h"

#undef interface
#include "RmlUi/Core.h"
#include "RmlUi/Debugger.h"
#include "RmlUi/Lua.h"
#include "LottiePlugin.h"
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
		OS::PrintError(message);
		return false;
		break;
	case Rml::Log::LT_ASSERT:
		OS::Print(message);
		return false;
		break;
	case Rml::Log::LT_WARNING:
		OS::PrintWarning(message);
		break;
	case Rml::Log::LT_INFO:
		OS::Print(message);
		break;
	default:
		break;
	}

	return true;
}

UISystem::UISystem()
    : System("UISystem", UpdateOrder::UI, true)
    , m_Context(nullptr)
    , m_Binder(this)
{
	m_Binder.bind(RootexEvents::UISystemEnableDebugger, &UISystem::enableDebugger);
	m_Binder.bind(RootexEvents::UISystemDisableDebugger, &UISystem::disableDebugger);
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
		PRINT("Loaded document: " + document->GetSourceURL());
		document->Show();
	}
	else
	{
		WARN("Could not load document: " + path);
	}
	return document;
}

void UISystem::unloadDocument(Rml::ElementDocument*& document)
{
	PRINT("Queued for unloading: " + document->GetSourceURL());
	m_Context->UnloadDocument(document);
	document = nullptr;
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

	m_FlipbookInstancer.reset(new FlipbookDecoratorInstancer());
	Rml::Factory::RegisterDecoratorInstancer("flipbook", m_FlipbookInstancer.get());

	Rml::Lua::Initialise(LuaInterpreter::GetSingleton()->getLuaState().lua_state());
	Rml::Lottie::Initialise();

	loadFont("rootex/assets/fonts/Lato-Regular.ttf");

	m_Context = Rml::CreateContext("default", Rml::Vector2i(systemData["width"], systemData["height"]));

	Rml::Debugger::Initialise(m_Context);

	InputInterface::GetSingleton()->initialise();
	InputInterface::GetSingleton()->setContext(m_Context);

	return true;
}

void UISystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	m_Context->Update();

	RootexDecorator::UpdateAll(deltaMilliseconds * MS_TO_S);

	RenderingDevice::GetSingleton()->setAlphaBS();
	RenderingDevice::GetSingleton()->setTemporaryUIRS();
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

void UISystem::draw()
{
	System::draw();

	ImGui::Text("Press F8 in game for UI debugger");
	static bool debugger = false;
	if (ImGui::Checkbox("Debugger", &debugger))
	{
		setDebugger(debugger);
	}

	bool enabled = InputInterface::GetSingleton()->m_IsEnabled;
	if (ImGui::Checkbox("Take Inputs", &enabled))
	{
		InputInterface::GetSingleton()->m_IsEnabled = enabled;
	}
}
