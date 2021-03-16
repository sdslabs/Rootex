
#include "TestHelper.h"
#include "test.h"

#include <Utils/LLGI.CommandListPool.h>
#include <array>
#include <fstream>
#include <iostream>
#include <map>

enum class SingleRectangleTestMode
{
	Triangle,
	Line,
	Point,
};

enum class SimpleTextureRectangleTestMode
{
	RGBA8,
	RGBA32F,
	R8,
};

void test_simple_rectangle(LLGI::DeviceType deviceType, SingleRectangleTestMode mode)
{
	if (mode == SingleRectangleTestMode::Point)
	{
		if (deviceType == LLGI::DeviceType::Default)
			return;
		if (deviceType == LLGI::DeviceType::Vulkan)
			return;
	}

	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("SimpleRectangle", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());

	LLGI::SafeAddRef(platform);

	auto graphics = platform->CreateGraphics();
	graphics->SetDisposed([platform]() -> void { platform->Release(); });

	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	auto commandListPool = std::make_shared<LLGI::CommandListPool>(graphics, sfMemoryPool, 3);

	std::shared_ptr<LLGI::Shader> shader_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_ps = nullptr;

	TestHelper::CreateShader(graphics, deviceType, "simple_rectangle.vert", "simple_rectangle.frag", shader_vs, shader_ps);

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F(-0.5, 0.5, 0.5),
								LLGI::Vec3F(0.5, -0.5, 0.5),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb,
								ib);

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
			break;

		sfMemoryPool->NewFrame();

		LLGI::Color8 color;
		color.R = count % 255;
		color.G = 0;
		color.B = 0;
		color.A = 255;

		auto renderPass = platform->GetCurrentScreen(color, true, false); // TODO: isDepthClear is false, because it fails with dx12.
		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		if (pips.count(renderPassPipelineState) == 0)
		{
			auto pip = graphics->CreatePiplineState();
			pip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			pip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			pip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			pip->VertexLayoutNames[0] = "POSITION";
			pip->VertexLayoutNames[1] = "UV";
			pip->VertexLayoutNames[2] = "COLOR";
			pip->VertexLayoutCount = 3;

			if (mode == SingleRectangleTestMode::Triangle)
			{
				pip->Topology = LLGI::TopologyType::Triangle;
			}
			else if (mode == SingleRectangleTestMode::Line)
			{
				pip->Topology = LLGI::TopologyType::Line;
			}
			else if (mode == SingleRectangleTestMode::Point)
			{
				pip->Topology = LLGI::TopologyType::Point;
			}

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs.get());
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps.get());
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		auto commandList = commandListPool->Get();
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->Draw(2);
		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);

			if (mode == SingleRectangleTestMode::Triangle)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
					.Save("SimpleRender.BasicTriangle.png");
			}
			else if (mode == SingleRectangleTestMode::Line)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save("SimpleRender.BasicLine.png");
			}
			else if (mode == SingleRectangleTestMode::Point)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
					.Save("SimpleRender.BasicPoint.png");
			}
			break;
		}
	}

	pips.clear();

	graphics->WaitFinish();
	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);
}

void test_index_offset(LLGI::DeviceType deviceType)
{
	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("IndexOffset", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());
	LLGI::SafeAddRef(platform);

	auto graphics = platform->CreateGraphics();
	graphics->SetDisposed([platform]() -> void { platform->Release(); });

	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	std::array<LLGI::CommandList*, 3> commandLists;
	for (size_t i = 0; i < commandLists.size(); i++)
		commandLists[i] = graphics->CreateCommandList(sfMemoryPool);

	std::shared_ptr<LLGI::Shader> shader_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_ps = nullptr;

	TestHelper::CreateShader(graphics, deviceType, "simple_rectangle.vert", "simple_rectangle.frag", shader_vs, shader_ps);

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F(-0.5, 0.5, 0.5),
								LLGI::Vec3F(0.5, -0.5, 0.5),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb,
								ib);

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
			break;

		sfMemoryPool->NewFrame();

		LLGI::Color8 color;
		color.R = count % 255;
		color.G = 0;
		color.B = 0;
		color.A = 255;

		auto renderPass = platform->GetCurrentScreen(color, true, false); // TODO: isDepthClear is false, because it fails with dx12.
		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		if (pips.count(renderPassPipelineState) == 0)
		{
			auto pip = graphics->CreatePiplineState();
			pip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			pip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			pip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			pip->VertexLayoutNames[0] = "POSITION";
			pip->VertexLayoutNames[1] = "UV";
			pip->VertexLayoutNames[2] = "COLOR";
			pip->VertexLayoutCount = 3;

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs.get());
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps.get());
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		auto commandList = commandLists[count % commandLists.size()];
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get(), 2 * 3);
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->Draw(1);
		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);
			Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save("SimpleRender.IndexOffset.png");
			break;
		}
	}

	pips.clear();

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);
}

void test_simple_constant_rectangle(LLGI::ConstantBufferType type, LLGI::DeviceType deviceType)
{
	auto code_gl_vs = R"(
#version 440 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;

layout(binding = 0) uniform Block 
{
	vec4 u_offset;
};

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_uv;
out vec4 v_color;

void main()
{
	gl_Position.x  = a_position.x;
	gl_Position.y  = a_position.y;
	gl_Position.z  = a_position.z;
	gl_Position.w  = 1.0f;
	gl_Position = gl_Position + u_offset;
	v_uv = a_uv;
	v_color = a_color;
}

)";

	auto code_gl_ps = R"(
#version 440 core
#extension GL_NV_gpu_shader5:require

layout(binding = 0) uniform Block 
{
	vec4 u_offset;
};

in vec2 v_uv;
in vec4 v_color;

layout(location = 0) out vec4 color;

void main()
{
    color = v_color + u_offset;
}

)";

	auto compiler = LLGI::CreateCompiler(deviceType);

	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("ConstantRectangle", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());

	auto graphics = platform->CreateGraphics();
	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	std::array<LLGI::CommandList*, 3> commandLists;
	for (size_t i = 0; i < commandLists.size(); i++)
		commandLists[i] = graphics->CreateCommandList(sfMemoryPool);

	LLGI::ConstantBuffer* cb_vs = nullptr;
	LLGI::ConstantBuffer* cb_ps = nullptr;

	LLGI::Shader* shader_vs = nullptr;
	LLGI::Shader* shader_ps = nullptr;

	std::vector<LLGI::DataStructure> data_vs;
	std::vector<LLGI::DataStructure> data_ps;

	if (compiler == nullptr)
	{
		auto binary_vs = TestHelper::LoadData("simple_constant_rectangle.vert.spv");
		auto binary_ps = TestHelper::LoadData("simple_constant_rectangle.frag.spv");

		LLGI::DataStructure d_vs;
		LLGI::DataStructure d_ps;

		d_vs.Data = binary_vs.data();
		d_vs.Size = static_cast<int32_t>(binary_vs.size());
		d_ps.Data = binary_ps.data();
		d_ps.Size = static_cast<int32_t>(binary_ps.size());

		data_vs.push_back(d_vs);
		data_ps.push_back(d_ps);

		shader_vs = graphics->CreateShader(data_vs.data(), static_cast<int32_t>(data_vs.size()));
		shader_ps = graphics->CreateShader(data_ps.data(), static_cast<int32_t>(data_ps.size()));
	}
	else
	{
		LLGI::CompilerResult result_vs;
		LLGI::CompilerResult result_ps;

		if (platform->GetDeviceType() == LLGI::DeviceType::Metal || platform->GetDeviceType() == LLGI::DeviceType::DirectX12 ||
			platform->GetDeviceType() == LLGI::DeviceType::Vulkan)
		{
			auto code_vs = TestHelper::LoadData("simple_constant_rectangle.vert");
			auto code_ps = TestHelper::LoadData("simple_constant_rectangle.frag");
			code_vs.push_back(0);
			code_ps.push_back(0);

			compiler->Compile(result_vs, (const char*)code_vs.data(), LLGI::ShaderStageType::Vertex);
			compiler->Compile(result_ps, (const char*)code_ps.data(), LLGI::ShaderStageType::Pixel);
		}
		else
		{
			compiler->Compile(result_vs, code_gl_vs, LLGI::ShaderStageType::Vertex);
			compiler->Compile(result_ps, code_gl_ps, LLGI::ShaderStageType::Pixel);
		}

		for (auto& b : result_vs.Binary)
		{
			LLGI::DataStructure d;
			d.Data = b.data();
			d.Size = static_cast<int32_t>(b.size());
			data_vs.push_back(d);
		}

		for (auto& b : result_ps.Binary)
		{
			LLGI::DataStructure d;
			d.Data = b.data();
			d.Size = static_cast<int32_t>(b.size());
			data_ps.push_back(d);
		}

		shader_vs = graphics->CreateShader(data_vs.data(), static_cast<int32_t>(data_vs.size()));
		shader_ps = graphics->CreateShader(data_ps.data(), static_cast<int32_t>(data_ps.size()));
	}

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F(-0.5, 0.5, 0.5),
								LLGI::Vec3F(0.5, -0.5, 0.5),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb,
								ib);

	if (type == LLGI::ConstantBufferType::LongTime)
	{
		cb_vs = graphics->CreateConstantBuffer(sizeof(float) * 4);
		cb_ps = graphics->CreateConstantBuffer(sizeof(float) * 4);

		auto cb_vs_buf = (float*)cb_vs->Lock();
		cb_vs_buf[0] = 0.2f;
		cb_vs_buf[1] = 0.0f;
		cb_vs_buf[2] = 0.0f;
		cb_vs_buf[3] = 0.0f;
		cb_vs->Unlock();

		auto cb_ps_buf = (float*)cb_ps->Lock();
		cb_ps_buf[0] = 0.0f;
		cb_ps_buf[1] = -1.0f;
		cb_ps_buf[2] = -1.0f;
		cb_ps_buf[3] = 0.0f;
		cb_ps->Unlock();
	}

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
		{
			break;
		}

		sfMemoryPool->NewFrame();

		if (type == LLGI::ConstantBufferType::ShortTime)
		{
			cb_vs = sfMemoryPool->CreateConstantBuffer(sizeof(float) * 4);
			cb_ps = sfMemoryPool->CreateConstantBuffer(sizeof(float) * 4);

			auto cb_vs_buf = (float*)cb_vs->Lock();
			cb_vs_buf[0] = (count % 100) / 100.0f;
			cb_vs_buf[1] = 0.0f;
			cb_vs_buf[2] = 0.0f;
			cb_vs_buf[3] = 0.0f;
			cb_vs->Unlock();

			auto cb_ps_buf = (float*)cb_ps->Lock();
			cb_ps_buf[0] = 0.0f;
			cb_ps_buf[1] = -1.0f;
			cb_ps_buf[2] = -1.0f;
			cb_ps_buf[3] = 0.0f;
			cb_ps->Unlock();
		}

		LLGI::Color8 color;
		color.R = count % 255;
		color.G = 0;
		color.B = 0;
		color.A = 255;

		auto renderPass = platform->GetCurrentScreen(color, true, false); // TODO: isDepthClear is false, because it fails with dx12.
		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		if (pips.count(renderPassPipelineState) == 0)
		{
			auto pip = graphics->CreatePiplineState();
			pip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			pip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			pip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			pip->VertexLayoutNames[0] = "POSITION";
			pip->VertexLayoutNames[1] = "UV";
			pip->VertexLayoutNames[2] = "COLOR";
			pip->VertexLayoutCount = 3;

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs);
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps);
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		auto commandList = commandLists[count % commandLists.size()];
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->SetConstantBuffer(cb_vs, LLGI::ShaderStageType::Vertex);
		commandList->SetConstantBuffer(cb_ps, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (type == LLGI::ConstantBufferType::ShortTime)
		{
			LLGI::SafeRelease(cb_vs);
			LLGI::SafeRelease(cb_ps);
		}

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);

			if (type == LLGI::ConstantBufferType::LongTime)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
					.Save("SimpleRender.ConstantLT.png");
			}
			else
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
					.Save("SimpleRender.ConstantST.png");
			}

			break;
		}
	}

	pips.clear();

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(cb_vs);
	LLGI::SafeRelease(cb_ps);
	LLGI::SafeRelease(shader_vs);
	LLGI::SafeRelease(shader_ps);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);

	LLGI::SafeRelease(compiler);
}

void test_simple_texture_rectangle(LLGI::DeviceType deviceType, SimpleTextureRectangleTestMode mode)
{
	auto code_gl_vs = R"(
#version 440 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;

out gl_PerVertex
{
	vec4 gl_Position;
};

out vec2 v_uv;
out vec4 v_color;

void main()
{
	gl_Position.x  = a_position.x;
	gl_Position.y  = a_position.y;
	gl_Position.z  = a_position.z;
	gl_Position.w  = 1.0f;
	gl_Position = gl_Position;
	v_uv = a_uv;
	v_color = a_color;
}

)";

	auto code_gl_ps = R"(
#version 440 core
#extension GL_NV_gpu_shader5:require

in vec2 v_uv;
in vec4 v_color;
layout(binding = 0) uniform sampler2D mainTexture;

layout(location = 0) out vec4 color;

void main()
{
    color = v_color * texture(mainTexture, v_uv);
}

)";

	auto compiler = LLGI::CreateCompiler(deviceType);

	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("TextureRectangle", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());

	auto graphics = platform->CreateGraphics();
	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	std::array<LLGI::CommandList*, 3> commandLists;
	for (size_t i = 0; i < commandLists.size(); i++)
		commandLists[i] = graphics->CreateCommandList(sfMemoryPool);

	LLGI::TextureInitializationParameter texParam;

	if (mode == SimpleTextureRectangleTestMode::RGBA8)
	{
		texParam.Format = LLGI::TextureFormatType::R8G8B8A8_UNORM;
	}
	else if (mode == SimpleTextureRectangleTestMode::RGBA32F)
	{
		texParam.Format = LLGI::TextureFormatType::R32G32B32A32_FLOAT;
	}
	else if (mode == SimpleTextureRectangleTestMode::R8)
	{
		texParam.Format = LLGI::TextureFormatType::R8_UNORM;
	}

	texParam.Size = LLGI::Vec2I(256, 256);

	auto textureDrawn = graphics->CreateTexture(texParam);
	assert(textureDrawn->GetType() == LLGI::TextureType::Color);

	TestHelper::WriteDummyTexture(textureDrawn);

	LLGI::Shader* shader_vs = nullptr;
	LLGI::Shader* shader_ps = nullptr;

	std::vector<LLGI::DataStructure> data_vs;
	std::vector<LLGI::DataStructure> data_ps;

	auto dummy_cb = LLGI::CreateSharedPtr(graphics->CreateConstantBuffer(16));

	if (compiler == nullptr)
	{
		auto binary_vs = TestHelper::LoadData("simple_texture_rectangle.vert.spv");
		auto binary_ps = TestHelper::LoadData("simple_texture_rectangle.frag.spv");

		LLGI::DataStructure d_vs;
		LLGI::DataStructure d_ps;

		d_vs.Data = binary_vs.data();
		d_vs.Size = static_cast<int32_t>(binary_vs.size());
		d_ps.Data = binary_ps.data();
		d_ps.Size = static_cast<int32_t>(binary_ps.size());

		data_vs.push_back(d_vs);
		data_ps.push_back(d_ps);

		shader_vs = graphics->CreateShader(data_vs.data(), static_cast<int32_t>(data_vs.size()));
		shader_ps = graphics->CreateShader(data_ps.data(), static_cast<int32_t>(data_ps.size()));
	}
	else
	{
		LLGI::CompilerResult result_vs;
		LLGI::CompilerResult result_ps;

		if (platform->GetDeviceType() == LLGI::DeviceType::Metal || platform->GetDeviceType() == LLGI::DeviceType::DirectX12 ||
			platform->GetDeviceType() == LLGI::DeviceType::Vulkan)
		{
			auto code_vs = TestHelper::LoadData("simple_texture_rectangle.vert");
			auto code_ps = TestHelper::LoadData("simple_texture_rectangle.frag");
			code_vs.push_back(0);
			code_ps.push_back(0);

			compiler->Compile(result_vs, (const char*)code_vs.data(), LLGI::ShaderStageType::Vertex);
			compiler->Compile(result_ps, (const char*)code_ps.data(), LLGI::ShaderStageType::Pixel);
		}
		else
		{
			compiler->Compile(result_vs, code_gl_vs, LLGI::ShaderStageType::Vertex);
			compiler->Compile(result_ps, code_gl_ps, LLGI::ShaderStageType::Pixel);
		}

		for (auto& b : result_vs.Binary)
		{
			LLGI::DataStructure d;
			d.Data = b.data();
			d.Size = static_cast<int32_t>(b.size());
			data_vs.push_back(d);
		}

		for (auto& b : result_ps.Binary)
		{
			LLGI::DataStructure d;
			d.Data = b.data();
			d.Size = static_cast<int32_t>(b.size());
			data_ps.push_back(d);
		}

		shader_vs = graphics->CreateShader(data_vs.data(), static_cast<int32_t>(data_vs.size()));
		shader_ps = graphics->CreateShader(data_ps.data(), static_cast<int32_t>(data_ps.size()));
	}

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F(-0.5, 0.5, 0.5),
								LLGI::Vec3F(0.5, -0.5, 0.5),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb,
								ib);

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
		{
			break;
		}

		sfMemoryPool->NewFrame();

		LLGI::Color8 color;
		color.R = count % 255;
		color.G = 0;
		color.B = 0;
		color.A = 255;

		auto renderPass = platform->GetCurrentScreen(color, true, false); // TODO: isDepthClear is false, because it fails with dx12.
		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		if (pips.count(renderPassPipelineState) == 0)
		{
			auto pip = graphics->CreatePiplineState();
			pip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			pip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			pip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			pip->VertexLayoutNames[0] = "POSITION";
			pip->VertexLayoutNames[1] = "UV";
			pip->VertexLayoutNames[2] = "COLOR";
			pip->VertexLayoutCount = 3;

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs);
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps);
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		auto commandList = commandLists[count % commandLists.size()];
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		// commandList->SetConstantBuffer(dummy_cb.get(), LLGI::ShaderStageType::Vertex);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->SetTexture(
			textureDrawn, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);
		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);

			if (mode == SimpleTextureRectangleTestMode::RGBA8)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
					.Save("SimpleRender.TextureRGB8.png");
			}

			if (mode == SimpleTextureRectangleTestMode::RGBA32F)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
					.Save("SimpleRender.TextureRGB32F.png");
			}

			if (mode == SimpleTextureRectangleTestMode::R8)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save("SimpleRender.TextureR8.png");
			}
			break;
		}
	}

	pips.clear();

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(textureDrawn);
	LLGI::SafeRelease(shader_vs);
	LLGI::SafeRelease(shader_ps);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);

	LLGI::SafeRelease(compiler);
}

void test_instancing(LLGI::DeviceType deviceType)
{
	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("Instancing", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(pp, window.get()));
	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());

	auto sfMemoryPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));

	auto commandListPool = std::make_shared<LLGI::CommandListPool>(graphics.get(), sfMemoryPool.get(), 3);

	std::shared_ptr<LLGI::Shader> shader_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_ps = nullptr;

	TestHelper::CreateShader(graphics.get(), deviceType, "instancing.vert", "simple_rectangle.frag", shader_vs, shader_ps);

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics.get(),
								LLGI::Vec3F(-0.2f, 0.2f, 0.5f),
								LLGI::Vec3F(0.2f, -0.2f, 0.5f),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb,
								ib);

	auto cb = LLGI::CreateSharedPtr(graphics->CreateConstantBuffer(sizeof(float) * 4 * 10));

	if (auto buf = static_cast<float*>(cb->Lock()))
	{
		for (int32_t i = 0; i < 10; i++)
		{
			buf[i * 4 + 0] = i * 0.1f;
			buf[i * 4 + 1] = i * 0.1f;
			buf[i * 4 + 2] = 0.0f;
			buf[i * 4 + 3] = 0.0f;
		}
		cb->Unlock();
	}

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
			break;

		sfMemoryPool->NewFrame();

		LLGI::Color8 color;
		color.R = count % 255;
		color.G = 0;
		color.B = 0;
		color.A = 255;

		auto renderPass = platform->GetCurrentScreen(color, true, false); // TODO: isDepthClear is false, because it fails with dx12.
		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		if (pips.count(renderPassPipelineState) == 0)
		{
			auto pip = graphics->CreatePiplineState();
			pip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			pip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			pip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			pip->VertexLayoutNames[0] = "POSITION";
			pip->VertexLayoutNames[1] = "UV";
			pip->VertexLayoutNames[2] = "COLOR";
			pip->VertexLayoutCount = 3;

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs.get());
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps.get());
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		auto commandList = commandListPool->Get();
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->SetConstantBuffer(cb.get(), LLGI::ShaderStageType::Vertex);
		commandList->Draw(2, 5);
		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);

			// save
			std::string path = "SimpleRender.Instancing.png";
			Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save(path.c_str());
		}
	}

	pips.clear();
}

void test_vtf(LLGI::DeviceType deviceType)
{
	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("VTF", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(pp, window.get()));
	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());

	auto sfMemoryPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));

	auto commandListPool = std::make_shared<LLGI::CommandListPool>(graphics.get(), sfMemoryPool.get(), 3);

	std::shared_ptr<LLGI::Shader> shader_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_ps = nullptr;

	TestHelper::CreateShader(graphics.get(), deviceType, "vtf.vert", "simple_rectangle.frag", shader_vs, shader_ps);

	LLGI::TextureInitializationParameter texParam;
	texParam.Size = LLGI::Vec2I(16, 16);
	texParam.Format = LLGI::TextureFormatType::R8G8B8A8_UNORM;
	auto textureDrawn = LLGI::CreateSharedPtr(graphics->CreateTexture(texParam));

	auto texBuf = static_cast<LLGI::Color8*>(textureDrawn->Lock());
	for (size_t i = 0; i < 16 * 16; i++)
	{
		texBuf[i].R = 127;
		texBuf[i].G = 127;
		texBuf[i].B = 127;
		texBuf[i].A = 127;
	}
	textureDrawn->Unlock();

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics.get(),
								LLGI::Vec3F(-0.2f, 0.2f, 0.5f),
								LLGI::Vec3F(0.2f, -0.2f, 0.5f),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(255, 255, 255, 255),
								vb,
								ib);

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
			break;

		sfMemoryPool->NewFrame();

		LLGI::Color8 color;
		color.R = count % 255;
		color.G = 0;
		color.B = 0;
		color.A = 255;

		auto renderPass = platform->GetCurrentScreen(color, true, false);
		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		if (pips.count(renderPassPipelineState) == 0)
		{
			auto pip = graphics->CreatePiplineState();
			pip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			pip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			pip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			pip->VertexLayoutNames[0] = "POSITION";
			pip->VertexLayoutNames[1] = "UV";
			pip->VertexLayoutNames[2] = "COLOR";
			pip->VertexLayoutCount = 3;

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs.get());
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps.get());
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		auto commandList = commandListPool->Get();
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());
		commandList->SetTexture(
			textureDrawn.get(), LLGI::TextureWrapMode::Clamp, LLGI::TextureMinMagFilter::Linear, 0, LLGI::ShaderStageType::Vertex);
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->Draw(2, 5);
		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);

			// save
			std::string path = "SimpleRender.VTF.png";
			Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save(path.c_str());
		}
	}

	pips.clear();
}

TestRegister SimpleRender_BasicTriangle("SimpleRender.BasicTriangle", [](LLGI::DeviceType device) -> void {
	test_simple_rectangle(device, SingleRectangleTestMode::Triangle);
});

TestRegister SimpleRender_BasicLine("SimpleRender.BasicLine",
									[](LLGI::DeviceType device) -> void { test_simple_rectangle(device, SingleRectangleTestMode::Line); });

TestRegister SimpleRender_BasicPoint("SimpleRender.BasicPoint", [](LLGI::DeviceType device) -> void {
	test_simple_rectangle(device, SingleRectangleTestMode::Point);
});

TestRegister SimpleRender_IndexOffset("SimpleRender.IndexOffset", [](LLGI::DeviceType device) -> void { test_index_offset(device); });

TestRegister SimpleRender_ConstantLT("SimpleRender.ConstantLT", [](LLGI::DeviceType device) -> void {
	test_simple_constant_rectangle(LLGI::ConstantBufferType::LongTime, device);
});

TestRegister SimpleRender_ConstantST("SimpleRender.ConstantST", [](LLGI::DeviceType device) -> void {
	test_simple_constant_rectangle(LLGI::ConstantBufferType::ShortTime, device);
});

TestRegister SimpleRender_Tex_RGBA8("SimpleRender.Texture_RGBA8", [](LLGI::DeviceType device) -> void {
	test_simple_texture_rectangle(device, SimpleTextureRectangleTestMode::RGBA8);
});

TestRegister SimpleRender_Tex_RGBA32F("SimpleRender.Texture_RGBA32F", [](LLGI::DeviceType device) -> void {
	test_simple_texture_rectangle(device, SimpleTextureRectangleTestMode::RGBA32F);
});
TestRegister SimpleRender_Tex_R8("SimpleRender.Texture_R8", [](LLGI::DeviceType device) -> void {
	test_simple_texture_rectangle(device, SimpleTextureRectangleTestMode::R8);
});

TestRegister SimpleRender_Instansing("SimpleRender.Instansing", [](LLGI::DeviceType device) -> void { test_instancing(device); });

TestRegister SimpleRender_VTF("SimpleRender.VTF", [](LLGI::DeviceType device) -> void { test_vtf(device); });
