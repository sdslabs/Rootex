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
#include "core/renderer/Cube.h"

#include "framework/components/test_component.h"
#include "framework/entity_factory.h"
#include "framework/systems/debug_system.h"
#include "framework/systems/test_system.h"

#include "main/window.h"

#include "os/os.h"

#include "scene/cube_test_node.h"
#include "scene/scene.h"

#include "script/interpreter.h"

#include "vendor//SimpleMath/SimpleMath.h"

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
	BufferFormat bufferFormat;
	bufferFormat.push(VertexBufferElement::Type::POSITION, "POSITION");
	Shader* shader = ShaderLibrary::MakeShader("Default", L"VertexShader.cso", L"PixelShader.cso", bufferFormat);

	Cube cube, cube1;

	float width = windowLua["deltaX"];
	float height = windowLua["deltaY"];
	float maxX = 1.0f;
	float minZ = 0.5f;
	float maxZ = 10.0f;
	float seconds = 10.0f;

	Ptr<Renderer> renderer(new Renderer(width, height));

	Scene scene(width, height);

	//TEMP WORKAROUND
	EntityID id = testEntity->getID();

	//Ref<SceneNode> node(new CubeTestNode(testEntity->getID(), Material()));
	Ref<SceneNode> node(new SceneNode(id, "CubeTestNode", Matrix::Identity, nullptr, RenderPass::Global, Material(), new Cube()));
	//Ref<SceneNode> child(new CubeTestNode(testEntity->getID(), Material()));
	Ref<SceneNode> child( new SceneNode(id, "CubeTestNode", Matrix::Identity, nullptr, RenderPass::Global, Material(), new Cube()));
	child->setTransforms(Matrix::CreateTranslation({ 0.0f, 3.0f, 0.0f }), nullptr);
	node->addChild(child);
	scene.addChild(testEntity->getID(), node);
	//      Global - node - child
	//     /
	// Root- ...
	//     \ ...
	//     \ ...
	std::optional<int> ret = {};

	Matrix projection = Matrix::CreatePerspective(maxX, maxX * height / width, minZ, maxZ);

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

		node->setTransform(Matrix::CreateFromYawPitchRoll(yaw, pitch, roll) * Matrix::CreateTranslation(x, y, 0.0f));
		child->addTransform(Matrix::CreateFromYawPitchRoll(0.1f, 0.0f, 0.0f));
		
		/*
		DirectX::XMMATRIX model = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(x, y, 0.0f);
		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveLH(maxX, maxX * height / width, minZ, maxZ);
		VSConstantBuffer.m_MVP = model * view * projection;
		VSConstantBuffer.m_MVP = DirectX::XMMatrixTranspose(VSConstantBuffer.m_MVP);
		shader.setConstantBuffer(VSConstantBuffer);
		*/
		//renderer->draw(vertexBuffer, indexBuffer, shader);
		//cube.GetSpatialData(u, l, roll, yaw, pitch, projection);
		//cube.Update();
		//cube.Draw();

		//cube1.GetSpatialData(0, 0, 0, 0, 0, projection);
		//cube1.Update();
		//cube1.Draw();

		scene.render();

		window->swapBuffers();
	}

	return ret.value();
}
