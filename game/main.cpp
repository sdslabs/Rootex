#include "common/common.h"

#include "core/audio/audio_source.h"
#include "core/audio/audio_system.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/renderer/buffer_format.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/renderer.h"
#include "core/renderer/shader_library.h"
#include "core/renderer/material.h"
#include "core/renderer/vertex_buffer.h"
#include "core/resource_loader.h"

#include "framework/components/test_component.h"
#include "framework/entity_factory.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/test_system.h"

#include "main/window.h"

#include "os/os.h"

#include "scene/scene.h"
#include "scene/cube_test_node.h"

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

	VertexBuffer vertexBuffer({ { -1.0f, -1.0f, -1.0f },
	    { +1.0f, -1.0f, -1.0f },
	    { -1.0f, +1.0f, -1.0f },
	    { +1.0f, +1.0f, -1.0f },
	    { -1.0f, -1.0f, +1.0f },
	    { +1.0f, -1.0f, +1.0f },
	    { -1.0f, +1.0f, +1.0f },
	    { +1.0f, +1.0f, +1.0f } });

	IndexBuffer indexBuffer({ 0, 2, 1,
	    2, 3, 1,
	    1, 3, 5,
	    3, 7, 5,
	    2, 6, 3,
	    3, 6, 7,
	    4, 5, 7,
	    4, 7, 6,
	    0, 4, 2,
	    2, 4, 6,
	    0, 1, 4,
	    1, 5, 4 });

	BufferFormat bufferFormat;
	float width = windowLua["deltaX"];
	float height = windowLua["deltaY"];
	float maxX = 1.0f;
	float minZ = 0.5f;
	float maxZ = 10.0f;
	float seconds = 10.0f;
	VSConstantBuffer VSConstantBuffer;
	PSConstantBuffer PSConstantBuffer;
	PSConstantBuffer.m_Colors[0] = { 1.0f, 0.0f, 0.0f, 1.0f };
	PSConstantBuffer.m_Colors[1] = { 0.0f, 1.0f, 0.0f, 1.0f };
	PSConstantBuffer.m_Colors[2] = { 0.0f, 0.0f, 1.0f, 1.0f };
	PSConstantBuffer.m_Colors[3] = { 0.0f, 0.0f, 1.0f, 1.0f };
	PSConstantBuffer.m_Colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	PSConstantBuffer.m_Colors[5] = { 0.0f, 1.0f, 1.0f, 1.0f };
	Shader* shader = ShaderLibrary::MakeShader("Default", L"VertexShader.cso", L"PixelShader.cso", bufferFormat);
	shader->setConstantBuffer(VSConstantBuffer);
	shader->setConstantBuffer(PSConstantBuffer);
	Ptr<Renderer> renderer(new Renderer(windowLua["deltaX"], windowLua["deltaY"]));

	Scene scene(width, height);
	Material mat(VSConstantBuffer, PSConstantBuffer);
	
	Ref<SceneNode> node(new CubeTestNode(testEntity->getID(), mat));
	scene.addChild(testEntity->getID(), node);
	
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
		DirectX::XMMATRIX model = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f) * DirectX::XMMatrixTranslation(x, y, 0.0f);
		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveLH(maxX, maxX * height / width, minZ, maxZ);
		VSConstantBuffer.m_M = model;
		VSConstantBuffer.m_V = view;
		VSConstantBuffer.m_P = projection;
		shader->setConstantBuffer(VSConstantBuffer);
		//scene.render();
		renderer->draw(vertexBuffer, indexBuffer, shader);
		window->swapBuffers();
	}

	return ret.value();
}
