#include "common/common.h"

#include "core/audio/audio_system.h"
#include "core/renderer/window.h"
#include "core/resource_loader.h"
#include "core/resource_data_reserve.h"
#include "os/os.h"
#include "script/interpreter.h"

int main()
{
	OS::initialize();
	OS::printLine("Rootex Engine is starting: Build(" + OS::getBuildDate() + "|" + OS::getBuildTime() + ")");
	
	AudioSystem::GetSingleton()->initialize();

	Ref<ResourceFile> w = ResourceLoader::createResourceFile(DirectoryShortcut::ENGINE, "Test File", "test/abc.txt", ResourceFile::Type::WAV);
	Ref<AudioBuffer> audio(new AudioBuffer(w, 2000, true));
	Ref<AudioSource> source(new AudioSource(audio.get()));
	source->play();

	// Engine starts from build/game/.
	Ref<ResourceFile> r = ResourceLoader::createResourceFile(DirectoryShortcut::ENGINE, "Test File", "test/abc.txt", ResourceFile::Type::TXT); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	OS::printLine(r->getText());

	Ref<ResourceFile> windowSettings = ResourceLoader::createResourceFile(DirectoryShortcut::GAME, "Window load script", "assets/config/window.lua", ResourceFile::Type::LUA);
	OS::printLine(windowSettings->getText());

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

	ResourceDataReserve::clear();
	return ret;
}
