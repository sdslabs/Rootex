#include "application.h"

#include "audio/audio_system.h"
#include "core/input/input_manager.h"
#include "core/renderer/shader_library.h"
#include "script/interpreter.h"

Application::Application(const String& windowConfigFilePath)
{
	if (!initialize(windowConfigFilePath))
	{
		ERR("Rootex Application could not be initialized");
	}
}

Application::~Application()
{
	shutdown();
}

bool Application::initialize(String windowConfigFilePath)
{
	if (!OS::Initialize())
	{
		ERR("Game Operating System was not initialized");
		return false;
	}
	m_ThreadPool.initialize();

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

	m_Window->show();
	return true;
}

void Application::shutdown()
{
	AudioSystem::GetSingleton()->shutDown();
}
