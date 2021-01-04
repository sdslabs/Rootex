#include "application.h"

#include "framework/scene_loader.h"
#include "framework/ecs_factory.h"
#include "core/resource_loader.h"
#include "core/resource_files/lua_text_resource_file.h"
#include "core/input/input_manager.h"
#include "core/renderer/shader_library.h"
#include "core/renderer/material_library.h"
#include "script/interpreter.h"

#include "systems/audio_system.h"
#include "systems/physics_system.h"
#include "systems/input_system.h"
#include "systems/ui_system.h"
#include "systems/render_ui_system.h"
#include "systems/render_system.h"
#include "systems/script_system.h"
#include "systems/transform_animation_system.h"

#include "Tracy/Tracy.hpp"

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
	
	PANIC(!OS::ElevateThreadPriority(), "Could not elevate main thread priority");
	PRINT("Current main thread priority: " + std::to_string(OS::GetCurrentThreadPriority()));

	m_ApplicationSettings.reset(new ApplicationSettings(ResourceLoader::CreateTextResourceFile(settingsFile)));

	ECSFactory::Initialize();

	JSON::json& systemsSettings = m_ApplicationSettings->getJSON()["systems"];
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
	JSON::json& inputSystemSettings = systemsSettings["InputSystem"];
	inputSystemSettings["width"] = m_Window->getWidth();
	inputSystemSettings["height"] = m_Window->getHeight();
	InputSystem::GetSingleton()->initialize(inputSystemSettings);

	ShaderLibrary::MakeShaders();
	PhysicsSystem::GetSingleton()->initialize(systemsSettings["PhysicsSystem"]);
	
	JSON::json& uiSystemSettings = systemsSettings["UISystem"];
	uiSystemSettings["width"] = m_Window->getWidth();
	uiSystemSettings["height"] = m_Window->getHeight();
	UISystem::GetSingleton()->initialize(uiSystemSettings);

	RenderUISystem::GetSingleton();
	RenderSystem::GetSingleton();
	ScriptSystem::GetSingleton();
	TransformAnimationSystem::GetSingleton();
	AnimationSystem::GetSingleton();

	if (!AudioSystem::GetSingleton()->initialize(systemsSettings["AudioSystem"]))
	{
		ERR("Audio System was not initialized");
	}
	
	auto&& postInitialize = m_ApplicationSettings->find("postInitialize");
	if (postInitialize != m_ApplicationSettings->end())
	{
		LuaInterpreter::GetSingleton()->getLuaState().script(ResourceLoader::CreateLuaTextResourceFile(*postInitialize)->getString());
	}

	m_Window->show();	
}

Application::~Application()
{
	SceneLoader::GetSingleton()->destroyAllScenes();
	AudioSystem::GetSingleton()->shutDown();
	UISystem::GetSingleton()->shutDown();
	ShaderLibrary::DestroyShaders();
	EventManager::GetSingleton()->releaseAllEventListeners();
}

void Application::run()
{
	while (!m_Window->processMessages())
	{
		m_FrameTimer.reset();

		for (auto& systems : System::GetSystems())
		{
			for (auto& system : systems)
			{
				if (system->isActive())
				{
					system->update(m_FrameTimer.getLastFrameTime());
				}
			}
		}
		
		process(m_FrameTimer.getLastFrameTime());
		
		EventManager::GetSingleton()->dispatchDeferred();
		
		m_Window->swapBuffers();
		FrameMark;
	}

	EventManager::GetSingleton()->call("Application", "ApplicationExit", 0);
}

void Application::process(float deltaMilliseconds)
{
	// Unused process function, meaning app doesn't need 
	// extra logic running every frame apart from the systems running
}

void Application::end()
{
}

Vector<FilePath> Application::getLibrariesPaths()
{
	return OS::GetDirectoriesInDirectory("rootex/vendor/");
}
