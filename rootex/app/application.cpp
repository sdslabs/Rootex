#include "application.h"

#include "framework/systems/audio_system.h"
#include "core/resource_loader.h"
#include "core/input/input_manager.h"
#include "core/renderer/shader_library.h"
#include "script/interpreter.h"
#include "systems/physics_system.h"
#include <dxgidebug.h>

Application::Application()
{
	if (!OS::Initialize())
	{
		ERR("Application OS was not initialized");
	}
}

Application::~Application()
{
	AudioSystem::GetSingleton()->shutDown();
	EntityFactory::GetSingleton()->destroyEntities(false);
	ShaderLibrary::DestroyShaders();
}

bool Application::initialize(const JSON::json& projectJSON)
{
	if (!AudioSystem::GetSingleton()->initialize())
	{
		ERR("Audio System was not initialized");
	}
	
	LuaInterpreter::GetSingleton();
	PhysicsSystem::GetSingleton()->initialize();
	
	JSON::json windowJSON = projectJSON["window"];
	m_Window.reset(new Window(
	    windowJSON["x"],
	    windowJSON["y"],
	    windowJSON["width"],
	    windowJSON["height"],
	    windowJSON["title"],
	    windowJSON["isEditor"],
	    windowJSON["msaa"]));
	InputManager::GetSingleton()->initialize(windowJSON["width"], windowJSON["height"]);

	ShaderLibrary::MakeShaders();

	auto&& postInitialize = projectJSON.find("postInitialize");
	if (postInitialize != projectJSON.end())
	{
		LuaInterpreter::GetSingleton()->getLuaState().script(ResourceLoader::CreateLuaTextResourceFile(*postInitialize)->getString());
	}

	m_Window->show();
	return true;
}
