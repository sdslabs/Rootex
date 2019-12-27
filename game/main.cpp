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

#include "framework/components/test_component.h"
#include "framework/entity_factory.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/test_system.h"

#include "main/window.h"

#include "os/os.h"

#include "scene/cube_test_node.h"
#include "scene/scene.h"

#include "script/interpreter.h"

int main()
{
	OS::Initialize();
	OS::PrintLine("Rootex Engine is starting: Build(" + OS::GetBuildDate() + "|" + OS::GetBuildTime() + ")");

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

	LuaTextResourceFile* testEntityRes = ResourceLoader::CreateLuaTextResourceFile("game/assets/test/entity.lua");
	Ref<Entity> testEntity = EntityFactory::GetSingleton()->createEntity(testEntityRes);

	TestSystem testSystem;
	testSystem.update(0.0f);

	DebugSystem debugSystem;
	debugSystem.update(0.0f);

	LuaInterpreter inter;
	inter.loadExecuteScript(windowSettings);

	LuaVariable windowLua = inter.getGlobal("window");
	Ptr<Window> window(new Window(
	    windowLua["x"],
	    windowLua["y"],
	    windowLua["deltaX"],
	    windowLua["deltaY"],
	    windowLua["title"]));

	Shader* shader = ShaderLibrary::MakeShader("Default", L"VertexShader.cso", L"PixelShader.cso", BufferFormat());
	Ptr<Renderer> renderer(new Renderer(windowLua["deltaX"], windowLua["deltaY"]));

	float width = windowLua["deltaX"];
	float height = windowLua["deltaY"];
	Scene scene(width, height);

	Ref<SceneNode> node(new CubeTestNode(testEntity->getID(), Material()));
	Ref<SceneNode> child(new CubeTestNode(testEntity->getID(), Material()));
	child->setTransforms(DirectX::XMMatrixTranslationFromVector({ 0.0f, 3.0f, 0.0f }), nullptr);
	node->addChild(child);
	scene.addChild(testEntity->getID(), node);
	//      Global - node - child
	//     /
	// Root- ...
	//     \ ...
	//     \ ...
	std::optional<int> ret = {};
	while (true)
	{
		if (ret = window->processMessages())
			break;

		renderer->clear();

		AudioSystem::GetSingleton()->update();

		static float x = 0;
		static float y = 0;
		static float u = 0;
		static float l = 0;
		if (GetAsyncKeyState(VK_UP))
		{
			u += 0.01;
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			u += -0.01;
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			l += -0.01;
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			l += 0.01;
		}
		x -= l;
		y += u;

		node->setTransform(DirectX::XMMatrixTranslation(x, y, 0.0f));
		child->addTransform(DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.1f, 0.0f));

		scene.render();
		window->swapBuffers();
	}

	return ret.value();
}
