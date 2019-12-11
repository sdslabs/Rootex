#include "common/common.h"

#include "core/audio/audio_system.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/audio/audio_source.h"
#include "core/renderer/directx/window.h"
#include "core/resource_loader.h"
#include "core/resource_data_reserve.h"
#include "os/os.h"
#include "script/interpreter.h"

int main()
{
	OS::initialize();
	OS::printLine("Rootex Engine is starting: Build(" + OS::getBuildDate() + "|" + OS::getBuildTime() + ")");

	AudioSystem::GetSingleton()->initialize();

	AudioResourceFile* w = ResourceLoader::createAudioResourceFile("game/assets/lost_in_istanbul.wav");
	Ref<StreamingAudioBuffer> audio(new StreamingAudioBuffer(w));
	Ref<StreamingAudioSource> source(new StreamingAudioSource(audio.get()));
	source->setLooping(true);
	source->play();

	AudioResourceFile* w2 = ResourceLoader::createAudioResourceFile("game/assets/lost_in_istanbul.wav");
	Ref<StaticAudioBuffer> audio2(new StaticAudioBuffer(w2));
	Ref<StaticAudioSource> source2(new StaticAudioSource(audio2.get()));
	source2->setLooping(false);
	source2->play();

	/*while (true)
	{
	}*/

	//AudioSystem::GetSingleton()->update();
	// Engine starts from build/game/.
	TextResourceFile* r = ResourceLoader::createTextResourceFile("rootex/test/abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	OS::printLine(r->getString());

	LuaTextResourceFile* windowSettings = ResourceLoader::createLuaTextResourceFile("game/assets/config/window.lua");
	OS::printLine(windowSettings->getString());

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

	return ret;
}
