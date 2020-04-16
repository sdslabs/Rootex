#include "application.h"

#include "framework/systems/audio_system.h"
#include "core/resource_loader.h"
#include "core/input/input_manager.h"
#include "core/renderer/shader_library.h"
#include "script/interpreter.h"
#include "systems/physics_system.h"

Application::Application(const String& settingsFile)
{
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
	PhysicsSystem::GetSingleton()->initialize();
	
	JSON::json windowJSON = m_ApplicationSettings->getJSON()["window"];
	m_Window.reset(new Window(
	    windowJSON["x"],
	    windowJSON["y"],
	    windowJSON["width"],
	    windowJSON["height"],
	    windowJSON["title"],
	    windowJSON["isEditor"],
	    windowJSON["msaa"],
		windowJSON["fullScreen"]));
	InputManager::GetSingleton()->initialize(windowJSON["width"], windowJSON["height"]);

	ShaderLibrary::MakeShaders();

	auto&& postInitialize = m_ApplicationSettings->find("postInitialize");
	if (postInitialize != m_ApplicationSettings->end())
	{
		LuaInterpreter::GetSingleton()->getLuaState().script(ResourceLoader::CreateLuaTextResourceFile(*postInitialize)->getString());
	}

	m_Window->show();
}

Application::~Application()
{
	AudioSystem::GetSingleton()->shutDown();
	EntityFactory::GetSingleton()->destroyEntities(false);
	ShaderLibrary::DestroyShaders();
}
