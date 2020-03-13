#include "application.h"

#include "audio/audio_system.h"
#include "core/resource_loader.h"
#include "core/input/input_manager.h"
#include "core/renderer/shader_library.h"
#include "script/interpreter.h"
#include "systems/physics_system.h"

Application::Application(const String& windowConfigFilePath)
{
	if (!initialize(windowConfigFilePath))
	{
		ERR("Rootex Application could not be initialized");
	}
}

Application::~Application()
{
	AudioSystem::GetSingleton()->shutDown();
}

Ref<Entity> Application::addEntity(String path)
{
	return EntityFactory::GetSingleton()->createEntity(ResourceLoader::CreateTextResourceFile(path));
}

bool Application::initialize(String windowConfigFilePath)
{
	if (!OS::Initialize())
	{
		ERR("Game Operating System was not initialized");
		return false;
	}

	if (!AudioSystem::GetSingleton()->initialize())
	{
		ERR("Audio System was not initialized");
		return false;
	}

	LuaInterpreter::GetSingleton()->loadExecuteScript(ResourceLoader::CreateLuaTextResourceFile(windowConfigFilePath));
	LuaVariable windowLua = LuaInterpreter::GetSingleton()->getGlobal("window");
	m_Window.reset(new Window(
	    windowLua["x"],
	    windowLua["y"],
	    windowLua["width"],
	    windowLua["height"],
	    windowLua["title"],
	    windowLua["isEditor"],
	    windowLua["msaa"]));
	InputManager::GetSingleton()->initialize(windowLua["width"], windowLua["height"]);

	ShaderLibrary::MakeShaders();

	LuaVariable postInitialize = windowLua["postInitialize"];
	if (!postInitialize.isNil() && postInitialize.isString())
	{
		LuaInterpreter::GetSingleton()->loadExecuteScript(ResourceLoader::CreateLuaTextResourceFile(postInitialize));
	}

	PhysicsSystem::GetSingleton()->initialize();

	m_Window->show();
	return true;
}
