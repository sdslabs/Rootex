#include "application.h"

#include "framework/systems/audio_system.h"
#include "core/resource_loader.h"
#include "core/input/input_manager.h"
#include "core/renderer/shader_library.h"
#include "core/renderer/material_library.h"
#include "script/interpreter.h"
#include "systems/physics_system.h"
#include "systems/ui_system.h"

Application* Application::s_Singleton = nullptr;

Application* Application::GetSingleton()
{
	return s_Singleton;
}

Application::Application(const String& settingsFile)
{
	if (!s_Singleton)
	{
		s_Singleton = this;
	}
	else
	{
		ERR("More than one Rootex applications are active");
	}

	if (!OS::Initialize())
	{
		ERR("Application OS was not initialized");
	}

	m_ApplicationSettings.reset(new ApplicationSettings(ResourceLoader::CreateTextResourceFile(settingsFile)));

	if (!AudioSystem::GetSingleton()->initialize())
	{
		ERR("Audio System was not initialized");
	}
	
	LuaInterpreter::GetSingleton();
	
	JSON::json windowJSON = m_ApplicationSettings->getJSON()["window"];
	m_Window.reset(new Window(
	    windowJSON["x"],
	    windowJSON["y"],
	    OS::GetDisplayWidth(),
	    OS::GetDisplayHeight(),
	    windowJSON["title"],
	    windowJSON["isEditor"],
	    windowJSON["msaa"],
		windowJSON["fullScreen"]));
	InputManager::GetSingleton()->initialize(m_Window->getWidth(), m_Window->getHeight());

	ShaderLibrary::MakeShaders();
	MaterialLibrary::LoadMaterials();
	PhysicsSystem::GetSingleton()->initialize();
	UISystem::GetSingleton()->initialize(m_Window->getWidth(), m_Window->getHeight());

	auto&& postInitialize = m_ApplicationSettings->find("postInitialize");
	if (postInitialize != m_ApplicationSettings->end())
	{
		LuaInterpreter::GetSingleton()->getLuaState().script(ResourceLoader::CreateLuaTextResourceFile(*postInitialize)->getString());
	}

	m_Window->show();
}

Application::~Application()
{
	EntityFactory::GetSingleton()->destroyEntities(false);
	AudioSystem::GetSingleton()->shutDown();
	UISystem::GetSingleton()->shutdown();
	ShaderLibrary::DestroyShaders();
}
