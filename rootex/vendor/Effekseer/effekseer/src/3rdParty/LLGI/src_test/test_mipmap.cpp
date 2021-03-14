
#include "TestHelper.h"
#include "test.h"

#include <Utils/LLGI.CommandListPool.h>
#include <array>
#include <fstream>
#include <iostream>
#include <map>

void test_mipmap(LLGI::DeviceType deviceType)
{
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

	LLGI::TextureInitializationParameter texParam_mipmap;

	texParam_mipmap.Format = LLGI::TextureFormatType::R8G8B8A8_UNORM;

	texParam_mipmap.Size = LLGI::Vec2I(256, 256);

	auto textureDrawn = graphics->CreateTexture(texParam_mipmap);

	texParam_mipmap.MipMapCount = 5;
	auto textureDrawnMipmap = graphics->CreateTexture(texParam_mipmap);
	assert(textureDrawn->GetType() == LLGI::TextureType::Color);
	assert(textureDrawnMipmap->GetType() == LLGI::TextureType::Color);

	TestHelper::WriteDummyTexture(textureDrawn);
	TestHelper::WriteDummyTexture(textureDrawnMipmap);

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

	LLGI::Vec3F large_ul = LLGI::Vec3F(-0.5, 0.5, 0.5);
	LLGI::Vec3F large_lr = LLGI::Vec3F(0, 0, 0.5);
	LLGI::Vec3F middle_ul = LLGI::Vec3F(0, 0.25, 0.5);
	LLGI::Vec3F middle_lr = LLGI::Vec3F(0.25, 0.0, 0.5);
	LLGI::Vec3F small_ul = LLGI::Vec3F(0.25, 0.13F, 0.5);
	LLGI::Vec3F small_lr = LLGI::Vec3F(0.38F, 0, 0.5);

	LLGI::Vec3F offset = LLGI::Vec3F(0.0, 0.5F, 0.0);

	// left
	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics, large_ul, large_lr, LLGI::Color8(255, 255, 255, 255), LLGI::Color8(0, 255, 0, 255), vb, ib);

	std::shared_ptr<LLGI::VertexBuffer> vb2;
	std::shared_ptr<LLGI::IndexBuffer> ib2;
	TestHelper::CreateRectangle(graphics, middle_ul, middle_lr, LLGI::Color8(255, 255, 255, 255), LLGI::Color8(0, 255, 0, 255), vb2, ib2);

	std::shared_ptr<LLGI::VertexBuffer> vb3;
	std::shared_ptr<LLGI::IndexBuffer> ib3;
	TestHelper::CreateRectangle(graphics, small_ul, small_lr, LLGI::Color8(255, 255, 255, 255), LLGI::Color8(0, 255, 0, 255), vb3, ib3);

	std::shared_ptr<LLGI::VertexBuffer> vb4;
	std::shared_ptr<LLGI::IndexBuffer> ib4;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F::Sub(large_ul, offset),
								LLGI::Vec3F::Sub(large_lr, offset),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb4,
								ib4);

	std::shared_ptr<LLGI::VertexBuffer> vb5;
	std::shared_ptr<LLGI::IndexBuffer> ib5;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F::Sub(middle_ul, offset),
								LLGI::Vec3F::Sub(middle_lr, offset),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb5,
								ib5);

	std::shared_ptr<LLGI::VertexBuffer> vb6;
	std::shared_ptr<LLGI::IndexBuffer> ib6;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F::Sub(small_ul, offset),
								LLGI::Vec3F::Sub(small_lr, offset),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb6,
								ib6);

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

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs);
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps);
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		auto commandList = commandLists[count % commandLists.size()];
		commandList->Begin();
		commandList->GenerateMipMap(textureDrawnMipmap);
		commandList->BeginRenderPass(renderPass);
		// commandList->SetConstantBuffer(dummy_cb.get(), LLGI::ShaderStageType::Vertex);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->SetTexture(
			textureDrawnMipmap, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->SetVertexBuffer(vb2.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib2.get());
		commandList->SetTexture(
			textureDrawnMipmap, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->SetVertexBuffer(vb3.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib3.get());
		commandList->SetTexture(
			textureDrawnMipmap, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->SetVertexBuffer(vb4.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib4.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->SetTexture(
			textureDrawn, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->SetVertexBuffer(vb5.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib5.get());
		commandList->SetTexture(
			textureDrawn, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->SetVertexBuffer(vb6.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib6.get());
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

			auto textureMipmap = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(textureMipmap);
			std::string path = "SimpleRender.TextureRGB8_MipMap.png";
			Bitmap2D(data, textureMipmap->GetSizeAs2D().X, textureMipmap->GetSizeAs2D().Y, textureMipmap->GetFormat()).Save(path.c_str());
		}
	}

	pips.clear();

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(textureDrawn);
	LLGI::SafeRelease(textureDrawnMipmap);
	LLGI::SafeRelease(shader_vs);
	LLGI::SafeRelease(shader_ps);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);

	LLGI::SafeRelease(compiler);
}
TestRegister SimpleRender_Tex_MipMap_RGBA8("SimpleRender.Texture_MipMap_RGBA8",
										   [](LLGI::DeviceType device) -> void { test_mipmap(device); });
