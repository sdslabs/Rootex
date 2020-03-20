#include "application.h"

#include "audio/audio_system.h"
#include "core/resource_loader.h"
#include "core/input/input_manager.h"
#include "core/renderer/shader_library.h"
#include "script/interpreter.h"
#include "systems/physics_system.h"

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
}

bool Application::initialize(const JSON::json& projectJSON)
{
	if (!AudioSystem::GetSingleton()->initialize())
	{
		ERR("Audio System was not initialized");
	}
	
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
	
	LuaInterpreter::GetSingleton()->registerTypes();

	auto&& postInitialize = projectJSON.find("postInitialize");
	if (postInitialize != projectJSON.end())
	{
		LuaInterpreter::GetSingleton()->loadExecuteScript(ResourceLoader::CreateLuaTextResourceFile(*postInitialize));
	}

	m_Window->show();
	return true;
}
