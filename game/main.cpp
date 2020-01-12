#include "common/common.h"

#include "core/audio/audio_source.h"
#include "core/audio/audio_system.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"

#include "core/renderer/buffer_format.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/material.h"
#include "core/renderer/renderer.h"
#include "core/renderer/shader_library.h"
#include "core/renderer/vertex_buffer.h"

#include "core/resource_loader.h"
#include "core/event_manager.h"

#include "framework/components/visual/visual_component.h"
#include "framework/components/visual/diffuse_visual_component.h"
#include "framework/components/visual/visual_component_graph.h"
#include "framework/components/test_component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/entity_factory.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/test_system.h"
#include "framework/systems/render_system.h"

#include "main/window.h"

#include "os/os.h"
#include "os/thread.h"
#include "os/timer.h"

#include "script/interpreter.h"

int main()
{
	OS::Initialize();
	OS::PrintLine("Rootex Engine is starting: Build(" + OS::GetBuildDate() + "|" + OS::GetBuildTime() + ")");
	DirectX::SimpleMath::Vector2 v2(1.0f, 1.0f);
	OS::PrintLine(std::to_string(v2.Length()));
	AudioSystem::GetSingleton()->initialize();
	AudioResourceFile* w = ResourceLoader::CreateAudioResourceFile("game/assets/lost_in_istanbul.wav");
	Ref<StreamingAudioBuffer> audio(new StreamingAudioBuffer(w));
	Ref<StreamingAudioSource> source(new StreamingAudioSource(audio.get()));
	source->setLooping(true);
	source->play();

	GameObject* gameObject = new GameObject();
	Ref<Example> test_event(new Example());
	EventManager::GetSingleton()->addListener(gameObject, Example::s_ExampleID);
	EventManager::GetSingleton()->call(test_event);
	EventManager::GetSingleton()->deferredCall(test_event);

	AudioResourceFile* w2 = ResourceLoader::CreateAudioResourceFile("game/assets/lost_in_istanbul.wav");
	Ref<StaticAudioBuffer> audio2(new StaticAudioBuffer(w2));
	Ref<StaticAudioSource> source2(new StaticAudioSource(audio2.get()));
	source2->setLooping(false);

	TextResourceFile* r = ResourceLoader::CreateTextResourceFile("rootex/test/abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	OS::PrintLine(r->getString());

	LuaTextResourceFile* windowSettings = ResourceLoader::CreateLuaTextResourceFile("game/assets/config/window.lua");
	OS::PrintLine(windowSettings->getString());

	Vector<Ref<Task>> ju;
	Ref<DebugTask> db(new DebugTask());
	Ref<RenderTask> rb(new RenderTask());

	ju.push_back(db);
	ju.push_back(rb);

	ThreadPool th;
	th.initialize();
	th.submit(ju);
	th.submit(ju);
	th.shutdown();

	LuaInterpreter inter;
	inter.loadExecuteScript(windowSettings);

	LuaVariable windowLua = inter.getGlobal("window");
	Ptr<Window> window(new Window(
	    windowLua["x"],
	    windowLua["y"],
	    windowLua["deltaX"],
	    windowLua["deltaY"],
	    windowLua["title"]));

	ShaderLibrary::MakeShaders();

	LuaTextResourceFile* testEntityRes = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/entity.lua");
	Ref<Entity> testEntity = EntityFactory::GetSingleton()->createEntity(testEntityRes);
	Ref<Entity> testEntity2 = EntityFactory::GetSingleton()->createEntity(testEntityRes);
	
	testEntity->getComponent<HierarchyComponent>()->addChild(testEntity2.get());

	std::cout << testEntity->getID() << std::endl;
	std::cout << testEntity->getComponent<HierarchyComponent>()->m_Children.size() << std::endl;
	std::cout << testEntity2->getID() << std::endl;
	std::cout << testEntity->getComponent<HierarchyComponent>()->m_Children[0]->getID() << std::endl;
	testEntity->getComponent<HierarchyComponent>()->removeChild(testEntity2);
	std::cout << testEntity->getComponent<HierarchyComponent>()->m_Children.size() << std::endl;


	Ref<VisualComponentGraph> visualGraph(new VisualComponentGraph(windowLua["deltaX"], windowLua["deltaY"]));
	Ref<RenderSystem> renderSystem(new RenderSystem());
	
	LuaTextResourceFile* teapotEntity = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/teapot.lua");
	Ref<Entity> teapot = EntityFactory::GetSingleton()->createEntity(teapotEntity);
	
	visualGraph->addChild(teapot->getComponent<DiffuseVisualComponent>());
	
	std::optional<int> ret = {};
	FrameTimer frameTimer;
	LoggingScopeTimer gameScopedLogger("GameTime");
	while (true)
	{
		frameTimer.reset();

		if (ret = window->processMessages())
			break;

		AudioSystem::GetSingleton()->update();

		static float x = 0;
		static float y = 0;
		static float u = 0;
		static float l = 0;
		static float roll = 0;
		static float pitch = 0;
		static float yaw = 0;
		if (GetAsyncKeyState(VK_LEFT))
		{
			u += 0.01;
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			u += -0.01;
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			l += -0.01;
		}
		if (GetAsyncKeyState(VK_UP))
		{
			l += 0.01;
		}
		x -= l;
		y += u;

		if (GetAsyncKeyState(VK_NUMPAD7))
		{
			roll += 0.01;
		}
		if (GetAsyncKeyState(VK_NUMPAD4))
		{
			roll += -0.01;
		}
		if (GetAsyncKeyState(VK_NUMPAD8))
		{
			pitch += 0.01;
		}
		if (GetAsyncKeyState(VK_NUMPAD5))
		{
			pitch += -0.01;
		}
		if (GetAsyncKeyState(VK_NUMPAD9))
		{
			yaw += 0.01;
		}
		if (GetAsyncKeyState(VK_NUMPAD6))
		{
			yaw += -0.01;
		}
		if (GetAsyncKeyState('R'))
		{
			u = l = roll = pitch = yaw = 0;
		}
		x = l;
		y = u;

		teapot->getComponent<TransformComponent>()->setTransform(Matrix::CreateFromYawPitchRoll(yaw, pitch, roll) * Matrix::CreateTranslation(0, y, 0.0f) * Matrix::CreateScale(x));

		RenderSystem::GetSingleton()->render(visualGraph.get(), window.get());

		EventManager::GetSingleton()->tick();

		frameTimer.showFPS();
	}

	return ret.value();
}
