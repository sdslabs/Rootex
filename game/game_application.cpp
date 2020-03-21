#include "game_application.h"

#include "rootex/core/audio/audio_system.h"
#include "rootex/core/input/input_manager.h"
#include "rootex/core/resource_loader.h"
#include "rootex/script/interpreter.h"
#include "rootex/framework/systems/render_system.h"
#include "rootex/core/audio/static_audio_buffer.h"
#include "rootex/core/audio/audio_source.h"

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new GameApplication());
}

Variant GameApplication::onExitEvent(const Event* event)
{
	PostQuitMessage(0);
	return true;
}

GameApplication::GameApplication()
    : Application("game/project.lua")
{
	BIND_EVENT_MEMBER_FUNCTION("InputExit", onExitEvent);

	Ref<Entity> cube = addEntity("game/assets/test/teapot.lua");
	Ref<Entity> teapot = addEntity("game/assets/test/teapot.lua");
	cube->addChild(teapot);
	Ref<Entity> pointlight = addEntity("game/assets/test/point_light.lua");
	pointlight->getComponent<TransformComponent>()->addTransform(Matrix::CreateTranslation({ 0.0f, 0.0f, 2.0f }));
	teapot->addChild(pointlight);
	HierarchySystem::GetSingleton()->addChild(cube);
	static Ref<StaticAudioBuffer> audio(new StaticAudioBuffer(ResourceLoader::CreateAudioResourceFile("game/assets/jungle.wav")));
	static Ref<StaticAudioSource> audioSource(new StaticAudioSource(audio));
	audioSource->play();
}

GameApplication::~GameApplication()
{
}

void GameApplication::run()
{
	while (true)
	{
		m_FrameTimer.reset();
		
		if (m_Window->processMessages())
		{
			break;
		}

		RenderSystem::GetSingleton()->render();
		AudioSystem::GetSingleton()->update();
		InputManager::GetSingleton()->update();
		EventManager::GetSingleton()->dispatchDeferred();

		m_Window->swapBuffers();
		m_Window->clearCurrentTarget();
		
		m_FrameTimer.showFPS();
	}
}

void GameApplication::shutDown()
{
}
