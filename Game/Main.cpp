#include "common/common.h"

#include "core/audio/audio_manager.h"
#include "core/audio/openal/openal_audio_manager.h"
#include "core/renderer/window.h"
#include "core/resource_loader.h"
#include "core/resource_manager.h"
#include "os/os.h"
#include "script/interpreter.h"

int main()
{
	printLine("Rootex Engine is starting: Build(" + OS::GetSingleton().getBuildDate() + "|" + OS::GetSingleton().getBuildTime() + ")");
	
	AudioManager::SetSingleton(new AudioManagerOpenAL());
	AudioManager::GetSingleton()->initialize();
	
	// Engine starts from build/game/.
	Ref<TextResource> r = ResourceLoader::createTextResourceFile(DirectoryShortcut::ENGINE, "Test File", "test/abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	printLine(r->getText());

	Ref<LuaScriptResource> windowSettings = ResourceLoader::createLuaScriptResourceFile(DirectoryShortcut::GAME, "Window load script", "assets/config/window.lua");
	printLine(windowSettings->getText());

	LuaInterpreter inter;
	inter.loadExecuteScript(windowSettings);
	LuaVariable window = inter.getGlobal("window");
	GameWindow* gameWindow = new GameWindow(
	    window["x"],
	    window["y"],
	    window["deltaX"],
	    window["deltaY"],
	    window["title"]);
	int ret = gameWindow->gameLoop();
	delete gameWindow;

	ResourceManager::empty();
	return ret;
}
