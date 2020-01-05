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

#include "framework/components/visual/visual_component.h"
#include "framework/components/visual/visual_component_graph.h"
#include "framework/components/test_component.h"
#include "framework/entity_factory.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/test_system.h"
#include "framework/systems/render_system.h"

#include "main/window.h"

#include "os/os.h"
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

	AudioResourceFile* w2 = ResourceLoader::CreateAudioResourceFile("game/assets/lost_in_istanbul.wav");
	Ref<StaticAudioBuffer> audio2(new StaticAudioBuffer(w2));
	Ref<StaticAudioSource> source2(new StaticAudioSource(audio2.get()));
	source2->setLooping(false);

	TextResourceFile* r = ResourceLoader::CreateTextResourceFile("rootex/test/abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	OS::PrintLine(r->getString());

	LuaTextResourceFile* windowSettings = ResourceLoader::CreateLuaTextResourceFile("game/assets/config/window.lua");
	OS::PrintLine(windowSettings->getString());

	LuaInterpreter inter;
	inter.loadExecuteScript(windowSettings);

	LuaVariable windowLua = inter.getGlobal("window");
	Ptr<Window> window(new Window(
	    windowLua["x"],
	    windowLua["y"],
	    windowLua["deltaX"],
	    windowLua["deltaY"],
	    windowLua["title"]));
	BufferFormat bufferFormat;
	bufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
	Shader* shader = ShaderLibrary::MakeShader("Default", L"VertexShader.cso", L"PixelShader.cso", bufferFormat);

	Ref<VisualComponentGraph> visualGraph(new VisualComponentGraph(windowLua["deltaX"], windowLua["deltaY"]));
	Ref<RenderSystem> renderSystem(new RenderSystem());
	
	LuaTextResourceFile* testCubeFile = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/cube_entity.lua");
	LuaTextResourceFile* testCubeChildFile = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/cube_entity.lua");
	Ref<Entity> testCube = EntityFactory::GetSingleton()->createEntity(testCubeFile);
	Ref<Entity> testCubeChild = EntityFactory::GetSingleton()->createEntity(testCubeChildFile);
	testCubeChild->getComponent<VisualComponent>()->setTransform(Matrix::CreateTranslation({ 0.0f, 3.0f, 0.0f }));

	visualGraph->addChild(testCube->getComponent<VisualComponent>());
	testCube->getComponent<VisualComponent>()->addChild(testCubeChild->getComponent<VisualComponent>());

	//TEMP WORKAROUND
	//EntityID id = testEntity->getID();

	//Ref<SceneNode> node(new CubeTestNode(testEntity->getID(), Material()));
	//Ref<SceneNode> node(new SceneNode(id, "CubeTestNode", Matrix::Identity, nullptr, RenderPass::Global, Material(), new DefaultCubeRenderableObject()));
	//Ref<SceneNode> child(new CubeTestNode(testEntity->getID(), Material()));
	//Ref<SceneNode> child(new SceneNode(id, "CubeTestNode", Matrix::Identity, nullptr, RenderPass::Global, Material(), new DefaultCubeRenderableObject()));
	//child->setTransforms(Matrix::CreateTranslation({ 0.0f, 3.0f, 0.0f }), nullptr);
	//node->addChild(child);
	//scene->addChild(testEntity->getID(), node);
	//      Global - node - child
	//     /
	// Root- ...
	//     \ ...
	//     \ ...
	std::optional<int> ret = {};
	FrameTimer frameTimer;
	LoggingScopeTimer gameScopedLogger("Game");
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

		testCube->getComponent<TransformComponent>()->setTransform(Matrix::CreateFromYawPitchRoll(yaw, pitch, roll) * Matrix::CreateTranslation(x, y, 0.0f));
		testCubeChild->getComponent<TransformComponent>()->addTransform(Matrix::CreateFromYawPitchRoll(0.1f, 0.0f, 0.0f));

		RenderSystem::GetSingleton()->render(visualGraph.get(), window.get());
		frameTimer.showFPS();
	}

	return ret.value();
}
