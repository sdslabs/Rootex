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

int main()
{
	OS::Initialize();
	OS::PrintLine("Rootex Engine is starting: Build(" + OS::GetBuildDate() + "|" + OS::GetBuildTime() + ")");
	DirectX::SimpleMath::Vector2 v2(1.0f, 1.0f);
	OS::PrintLine(std::to_string(v2.Length()));
	AudioSystem::GetSingleton()->initialize();
	AudioResourceFile* w = ResourceLoader::CreateAudioResourceFile("game/assets/hipshop.wav");
	Ref<StreamingAudioBuffer> audio(new StreamingAudioBuffer(w));
	Ref<StreamingAudioSource> source(new StreamingAudioSource(audio.get()));
	source->setLooping(true);
	source->play();

	Ref<EventHandler> gameQuitter(new EventHandler());
	gameQuitter->setHandler([](const Event* event) { PostQuitMessage(0); });
	EventManager::GetSingleton()->addListener(gameQuitter.get(), Event::Type::InputExit);
	Ref<EventHandler> gameEventHandler(new EventHandler());
	EventManager::GetSingleton()->addListener(gameEventHandler.get(), Event::Type::Test);
	Ref<Event> testEvent(new Event("Test Event", Event::Type::Test, String("SDSLabs")));
	EventManager::GetSingleton()->call(testEvent.get()); // This should show warning because there is no handling function set for the event handler
	EventManager::GetSingleton()->deferredCall(testEvent);

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
		false));
	InputManager::GetSingleton()->initialize(windowLua["deltaX"], windowLua["deltaY"]);
	ShaderLibrary::MakeShaders();

	Ref<VisualComponentGraph> visualGraph(new VisualComponentGraph(windowLua["deltaX"], windowLua["deltaY"]));
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
	visualGraph->addChild(spotLight);

	LuaTextResourceFile* teapotEntity = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/teapot.lua");
	Ref<Entity> teapot = EntityFactory::GetSingleton()->createEntity(teapotEntity);

	Ref<Entity> teapotChild = EntityFactory::GetSingleton()->createEntity(teapotEntity);
	teapotChild->getComponent<DiffuseVisualComponent>()->setTransform(Matrix::CreateTranslation({ 0.0f, 1.0f, 0.0f }));
	teapot->getComponent<HierarchyComponent>()->addChild(teapotChild);
	teapot->setEventHandler([](const Event* event) {
		Vector2 inputData = InputManager::GetSingleton()->getMousePositionDelta();
		OS::PrintLine("Event received: " + std::to_string(inputData.x) + "," + std::to_string(inputData.y));
	});
	teapot->subscribe(Event::Type::InputForward);
	teapot->subscribe(Event::Type::InputBackward);
	teapot->subscribe(Event::Type::InputLeft);
	teapot->subscribe(Event::Type::InputRight);
	visualGraph->addChild(teapot);

	std::optional<int> ret = {};
	FrameTimer frameTimer;
	LoggingScopeTimer gameScopedLogger("GameTime");

	teapot->getComponent<TransformComponent>()->setTransform(Matrix::CreateFromYawPitchRoll(0, 0, 0) * Matrix::CreateTranslation(0, 0, -5.0f) * Matrix::CreateScale(1));
	while (true)
	{
		frameTimer.reset();

		if (ret = window->processMessages())
			break;

		AudioSystem::GetSingleton()->update();

		static float xp = 1;
		static float yp = 0;
		static float zp = 2;

		static float x = 1;
		static float y = 0;
		static float z = 0;
		static float u = 0;
		static float l = 0;
		static float in = -2;
		static float roll = 0;
		static float pitch = 0;
		static float yaw = 3.14;
		if (GetAsyncKeyState(VK_LEFT))
		{
			l += 0.1;
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			l += -0.1;
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			u += -0.1;
		}
		if (GetAsyncKeyState(VK_UP))
		{
			u += 0.1;
		}
		//if (GetAsyncKeyState(VK_NUMPAD4))
		//{
		//	in += -0.1;
		//}
		//if (GetAsyncKeyState(VK_NUMPAD7))
		//{
		//	in += 0.1;
		//}
		
		if (GetAsyncKeyState(VK_NUMPAD7))
		{
			roll += 0.1;
		}
		if (GetAsyncKeyState(VK_NUMPAD4))
		{
			roll += -0.1;
		}
		if (GetAsyncKeyState(VK_NUMPAD8))
		{
			pitch += 0.1;
		}
		if (GetAsyncKeyState(VK_NUMPAD5))
		{
			pitch += -0.1;
		}
		if (GetAsyncKeyState(VK_NUMPAD9))
		{
			yaw += 0.1;
		}
		if (GetAsyncKeyState(VK_NUMPAD6))
		{
			yaw += -0.1;
		}
		if (GetAsyncKeyState('R'))
		{
			u = l = roll = pitch = yaw = 0;
		}

		if (GetAsyncKeyState('W'))
		{
			yp += 1.0;
		}
		if (GetAsyncKeyState('S'))
		{
			yp -= 1.0;
		}
		if (GetAsyncKeyState('A'))
		{
			xp -= 1.0;
		}
		if (GetAsyncKeyState('D'))
		{
			xp += 1.0;
		}
		if (GetAsyncKeyState('Q'))
		{
			zp += 1.0;
		}
		if (GetAsyncKeyState('E'))
		{
			zp -= 1.0;
		}

		x = l;
		y = u;
		z = in;
		spotLight->getComponent<TransformComponent>()->setPosition(Vector3(xp, yp, zp));
		spotLight->getComponent<TransformComponent>()->setRotation(yaw, pitch, roll);
		//spotLight->getComponent<TransformComponent>()->setPosition({ xp, yp, zp });

		teapot->getComponent<TransformComponent>()->setTransform(Matrix::CreateFromYawPitchRoll(yaw, pitch, roll) * Matrix::CreateTranslation(0, y, 0.0f) * Matrix::CreateScale(x));
		
		RenderSystem::GetSingleton()->render(visualGraph.get());
		//LightSystem::GetSingleton()->apply();

		x += 0.1f * (InputManager::GetSingleton()->isPressed(Event::Type::InputForward) - InputManager::GetSingleton()->isPressed(Event::Type::InputBackward));
		y += 0.1f * (InputManager::GetSingleton()->isPressed(Event::Type::InputRight) - InputManager::GetSingleton()->isPressed(Event::Type::InputLeft));
		visualGraph->getCamera()->setPosition({ y, 0.0f, -x });
		
		RenderSystem::GetSingleton()->render(visualGraph.get());
		InputManager::GetSingleton()->update();
		EventManager::GetSingleton()->tick();

		//frameTimer.showFPS();
		window->swapBuffers();
	}

	return ret.value();
	}
