#include "common/common.h"

#include "core/audio/audio_source.h"
#include "core/audio/audio_system.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"

#include "core/input/input_manager.h"

#include "core/renderer/buffer_format.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/material.h"
#include "core/renderer/renderer.h"
#include "core/renderer/shader_library.h"
#include "core/renderer/vertex_buffer.h"

#include "core/event_manager.h"
#include "core/resource_loader.h"

#include "framework/components/hierarchy_component.h"
#include "framework/components/test_component.h"
#include "framework/components/visual/diffuse_visual_component.h"
#include "framework/components/visual/visual_component.h"
#include "framework/components/visual/visual_component_graph.h"
#include "framework/entity_factory.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "framework/systems/test_system.h"

#include "main/window.h"

#include "os/os.h"
#include "os/thread.h"
#include "os/timer.h"

#include "script/interpreter.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

Variant function(const Event* event) { return true; };

int main()
{
	OS::Initialize();
	OS::PrintLine("Rootex Engine is starting: Build(" + OS::GetBuildDate() + "|" + OS::GetBuildTime() + ")");
	DirectX::SimpleMath::Vector2 v2(1.0f, 1.0f);
	OS::PrintLine(std::to_string(v2.Length()));
	AudioSystem::GetSingleton()->initialize();
	AudioResourceFile* w = ResourceLoader::CreateAudioResourceFile("game/assets/hipshop.wav");
	Ref<StreamingAudioBuffer> audio(new StreamingAudioBuffer(w));
	Ref<StreamingAudioSource> source(new StreamingAudioSource(audio));
	source->setLooping(true);
	//source->play();

	BIND_EVENT_FUNCTION("InputExit", [](const Event* event) -> Variant {
		PostQuitMessage(0);
		return true;
	});

	EventManager::GetSingleton()->addListener("InputBackward", CreateDelegate([source](const Event* event) -> Variant {
		if (std::get<Vector2>(event->getData()).x)
			return nullptr;
		if (source->isPaused())
		{
			source->play();
		}
		else
		{
			source->pause();
		}
		return true;
	}));

	EventManager::GetSingleton()->call("Test Event", "TestEvent", String("SDSLabs"));
	EventManager::GetSingleton()->deferredCall("Test Event", "TestEvent", String("SDSLabs"));

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
	    windowLua["title"],
	    false,
	    windowLua["msaa"]));
	InputManager::GetSingleton()->initialize(windowLua["deltaX"], windowLua["deltaY"]);
	ShaderLibrary::MakeShaders();

	Ref<VisualComponentGraph> visualGraph(new VisualComponentGraph());
	Ref<RenderSystem> renderSystem(new RenderSystem());
	Ref<LightSystem> lightSystem(new LightSystem());

	LuaTextResourceFile* spotLightEntity = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/spot_light.lua");
	Ref<Entity> spotLight = EntityFactory::GetSingleton()->createEntity(spotLightEntity);
	//LuaTextResourceFile* directionalLightEntity = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/directional_light.lua");
	//Ref<Entity> directionalLight = EntityFactory::GetSingleton()->createEntity(directionalLightEntity);

	//LuaTextResourceFile* teapotEntity = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/teapot.lua");
	//Ref<Entity> teapot = EntityFactory::GetSingleton()->createEntity(teapotEntity);

	//Ref<Entity> teapotChild = EntityFactory::GetSingleton()->createEntity(teapotEntity);
	//teapotChild->getComponent<DiffuseVisualComponent>()->setTransform(Matrix::CreateTranslation({ 0.0f, 1.0f, 0.0f }));
	//teapot->getComponent<HierarchyComponent>()->addChild(teapotChild);

	//visualGraph->addChild(teapot);
	//visualGraph->addChild(spotLight);

	LuaTextResourceFile* teapotEntity = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/teapot.lua");
	Ref<Entity> teapot = EntityFactory::GetSingleton()->createEntity(teapotEntity);

	Ref<Entity> teapotChild = EntityFactory::GetSingleton()->createEntity(teapotEntity);
	teapotChild->getComponent<DiffuseVisualComponent>()->setTransform(Matrix::CreateTranslation({ 0.0f, 1.0f, 0.0f }));
	//teapot->getComponent<HierarchyComponent>()->addChild(teapotChild);


	BIND_EVENT_FUNCTION("InputForward", function);
	BIND_EVENT_FUNCTION("InputForward", [&](const Event* event) -> Variant
	{
		Vector2 keyData = std::get<Vector2>(event->getData());
		OS::PrintLine("Event received: " + std::to_string(keyData.x) + "," + std::to_string(keyData.y));
		return true;
	});
	
	visualGraph->addChild(teapot);

	std::optional<int> ret = {};
	FrameTimer frameTimer;
	LoggingScopeTimer gameScopedLogger("GameTime");

	teapot->getComponent<TransformComponent>()->setTransform(Matrix::CreateFromYawPitchRoll(0, 0, 0) * Matrix::CreateTranslation(0, 0, -5.0f) * Matrix::CreateScale(1));
	window->show();
	while (true)
	{
		frameTimer.reset();

		if (ret = window->processMessages())
			break;

		AudioSystem::GetSingleton()->update();
		static float x = 0;
		static float dx = 0;
		static float y = 0;
		static float dy = 0;
		dx += 0.001f * (InputManager::GetSingleton()->isPressed("InputForward") - InputManager::GetSingleton()->isPressed("InputBackward"));
		dy += 0.001f * (InputManager::GetSingleton()->isPressed("InputRight") - InputManager::GetSingleton()->isPressed("InputLeft"));
		visualGraph->getCamera()->setPosition({ y += dy, 0.0f, -(x += dx) });

		RenderSystem::GetSingleton()->render(visualGraph.get());
		InputManager::GetSingleton()->update();
		EventManager::GetSingleton()->dispatchDeferred();

		//frameTimer.showFPS();
		window->swapBuffers();
		window->clearCurrentTarget();
	}

	AudioSystem::GetSingleton()->shutDown();

	return ret.value();
}
