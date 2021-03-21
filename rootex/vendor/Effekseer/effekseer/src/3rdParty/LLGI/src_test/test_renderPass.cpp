#include "TestHelper.h"
#include "test.h"
#include <array>
#include <map>

enum class RenderPassTestMode
{
	None,
	MSAA,
	MSAADepth,
	CopyTexture,
};

void test_renderPass(LLGI::DeviceType deviceType, RenderPassTestMode mode)
{
#if !(defined(__linux__) || defined(__APPLE__) || defined(WIN32))
	if (mode == RenderPassTestMode::MSAA || mode == RenderPassTestMode::MSAADepth)
	{
		return;
	}
#endif

	bool isMSAATest = mode == RenderPassTestMode::MSAA || mode == RenderPassTestMode::MSAADepth;

	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("RenderPass", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());

	auto graphics = platform->CreateGraphics();
	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	std::array<LLGI::CommandList*, 3> commandLists;
	for (size_t i = 0; i < commandLists.size(); i++)
		commandLists[i] = graphics->CreateCommandList(sfMemoryPool);

	LLGI::RenderTextureInitializationParameter params;
	params.Size = LLGI::Vec2I(256, 256);

	if (isMSAATest)
	{
		params.SamplingCount = 4;
	}

	auto renderTexture = graphics->CreateRenderTexture(params);
	assert(renderTexture->GetType() == LLGI::TextureType::Render);

	params.SamplingCount = 1;
	auto renderTextureDst = graphics->CreateRenderTexture(params);

	LLGI::Texture* depthTexture = nullptr;
	LLGI::Texture* depthTextureDst = nullptr;

	if (mode == RenderPassTestMode::MSAADepth)
	{
		LLGI::DepthTextureInitializationParameter depthParam;
		depthParam.Size = params.Size;
		depthParam.SamplingCount = 4;

		depthTexture = graphics->CreateDepthTexture(depthParam);

		LLGI::DepthTextureInitializationParameter depthParamDst;
		depthParamDst.Size = params.Size;

		depthTextureDst = graphics->CreateDepthTexture(depthParamDst);
	}

	LLGI::RenderPass* renderPass = nullptr;

	if (mode == RenderPassTestMode::MSAADepth)
	{
		if (graphics->IsResolvedDepthSupported())
		{
			renderPass = graphics->CreateRenderPass(renderTexture, renderTextureDst, depthTexture, depthTextureDst);
		}
		else
		{
			renderPass = graphics->CreateRenderPass(renderTexture, renderTextureDst, depthTexture, nullptr);
		}
	}
	else if (mode == RenderPassTestMode::MSAA)
	{
		renderPass = graphics->CreateRenderPass(renderTexture, renderTextureDst, nullptr, nullptr);
	}
	else
	{
		renderPass = graphics->CreateRenderPass(&renderTexture, 1, nullptr);
	}

	assert(renderPass->GetRenderTextureCount() == 1);

	LLGI::TextureInitializationParameter texParam;
	texParam.Size = LLGI::Vec2I(256, 256);
	auto texture = graphics->CreateTexture(texParam);

	auto texture_buf = (LLGI::Color8*)texture->Lock();
	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			texture_buf[x + y * 256].R = 128;
			texture_buf[x + y * 256].G = 128;
			texture_buf[x + y * 256].B = 128;
			texture_buf[x + y * 256].A = 255;
		}
	}
	texture->Unlock();

	std::shared_ptr<LLGI::Shader> shader_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_ps = nullptr;

	TestHelper::CreateShader(graphics, deviceType, "simple_texture_rectangle.vert", "simple_texture_rectangle.frag", shader_vs, shader_ps);

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics,
								LLGI::Vec3F(-0.5, 0.5, 0.5),
								LLGI::Vec3F(0.5, -0.5, 0.5),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb,
								ib);

	std::shared_ptr<LLGI::VertexBuffer> vb2;
	{

		vb2 = LLGI::CreateSharedPtr(graphics->CreateVertexBuffer(sizeof(SimpleVertex) * 4));
		auto vb_buf = (SimpleVertex*)vb2->Lock();
		vb_buf[0].Pos = LLGI::Vec3F(-0.5f, 0.5f, 0.5f);
		vb_buf[1].Pos = LLGI::Vec3F(0.5f, 0.5f, 0.5f);
		vb_buf[2].Pos = LLGI::Vec3F(0.6f, -0.5f, 0.5f);
		vb_buf[3].Pos = LLGI::Vec3F(-0.6f, -0.5f, 0.5f);

		vb_buf[0].UV = LLGI::Vec2F(0.0f, 0.0f);
		vb_buf[1].UV = LLGI::Vec2F(1.0f, 0.0f);
		vb_buf[2].UV = LLGI::Vec2F(1.0f, 1.0f);
		vb_buf[3].UV = LLGI::Vec2F(0.0f, 1.0f);

		vb_buf[0].Color = LLGI::Color8(255, 255, 255, 255);
		vb_buf[1].Color = LLGI::Color8(255, 255, 255, 255);
		vb_buf[2].Color = LLGI::Color8(255, 255, 255, 255);
		vb_buf[3].Color = LLGI::Color8(255, 255, 255, 255);

		vb2->Unlock();
	}

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
		{
			break;
		}

		sfMemoryPool->NewFrame();

		LLGI::Color8 color1;
		color1.R = 0;
		color1.G = count % 255;
		color1.B = 0;
		color1.A = 255;
		renderPass->SetIsColorCleared(true);
		renderPass->SetClearColor(color1);

		LLGI::Color8 color2;
		color2.R = count % 255;
		color2.G = 0;
		color2.B = 0;
		color2.A = 255;

		auto commandList = commandLists[count % commandLists.size()];
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->SetVertexBuffer(vb2.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());

		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		auto renderPassSc = platform->GetCurrentScreen(color2, true);
		auto renderPassPipelineStateSc = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPassSc));

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

		// Render to RenderTargetTexture
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->SetTexture(
			texture, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->EndRenderPass();

		if (mode == RenderPassTestMode::CopyTexture)
		{
			commandList->CopyTexture(renderTexture, renderTextureDst);
		}

		commandList->BeginRenderPass(platform->GetCurrentScreen(color2, true));
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());

		if (pips.count(renderPassPipelineStateSc) == 0)
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
			pip->SetRenderPassPipelineState(renderPassPipelineStateSc.get());
			pip->Compile();

			pips[renderPassPipelineStateSc] = LLGI::CreateSharedPtr(pip);
		}

		// Render to backbuffer
		commandList->SetPipelineState(pips[renderPassPipelineStateSc].get());

		if (mode == RenderPassTestMode::CopyTexture || (isMSAATest))
		{
			commandList->SetTexture(
				renderTextureDst, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		}
		else
		{
			commandList->SetTexture(
				renderTexture, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		}

		commandList->Draw(2);
		commandList->EndRenderPass();

		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto screenTex = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(screenTex);

			if (mode == RenderPassTestMode::MSAA)
			{
				Bitmap2D(data, screenTex->GetSizeAs2D().X, screenTex->GetSizeAs2D().Y, screenTex->GetFormat()).Save("RenderPass.MSAA.png");
			}
			else if (mode == RenderPassTestMode::None)
			{
				Bitmap2D(data, screenTex->GetSizeAs2D().X, screenTex->GetSizeAs2D().Y, screenTex->GetFormat()).Save("RenderPass.Basic.png");
			}
			else
			{
				Bitmap2D(data, screenTex->GetSizeAs2D().X, screenTex->GetSizeAs2D().Y, screenTex->GetFormat())
					.Save("RenderPass.CopyTexture.png");
			}

			if (mode == RenderPassTestMode::None)
			{
				auto data2 = graphics->CaptureRenderTarget(renderTexture);
				Bitmap2D(data2, renderTexture->GetSizeAs2D().X, renderTexture->GetSizeAs2D().Y, renderTexture->GetFormat())
					.Save("RenderPass.Basic.2.png");
			}
		}
	}

	pips.clear();

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(renderTexture);
	LLGI::SafeRelease(renderTextureDst);
	LLGI::SafeRelease(depthTexture);
	LLGI::SafeRelease(depthTextureDst);
	LLGI::SafeRelease(renderPass);
	LLGI::SafeRelease(texture);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);
}

void test_copyTextureToScreen(LLGI::DeviceType deviceType)
{
	if (deviceType != LLGI::DeviceType::DirectX12)
		return;

	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("RenderPass", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());

	auto graphics = platform->CreateGraphics();
	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	std::array<LLGI::CommandList*, 3> commandLists;
	for (size_t i = 0; i < commandLists.size(); i++)
		commandLists[i] = graphics->CreateCommandList(sfMemoryPool);

	LLGI::RenderTextureInitializationParameter params;
	params.Size = LLGI::Vec2I(1280, 720);

	auto renderTexture = graphics->CreateRenderTexture(params);
	assert(renderTexture->GetType() == LLGI::TextureType::Render);

	LLGI::RenderPass* renderPass = graphics->CreateRenderPass(&renderTexture, 1, nullptr);

	assert(renderPass->GetRenderTextureCount() == 1);

	while (count < 60)
	{
		if (!platform->NewFrame())
		{
			break;
		}

		sfMemoryPool->NewFrame();

		LLGI::Color8 color1;
		color1.R = 0;
		color1.G = count % 255;
		color1.B = 0;
		color1.A = 255;
		renderPass->SetIsColorCleared(true);
		renderPass->SetClearColor(color1);

		LLGI::Color8 color2;
		color2.R = count % 255;
		color2.G = 0;
		color2.B = 0;
		color2.A = 255;

		auto commandList = commandLists[count % commandLists.size()];
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->EndRenderPass();

		commandList->CopyTexture(renderTexture, platform->GetCurrentScreen(color2, true)->GetRenderTexture(0));

		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);

			Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
				.Save("RenderPass.CopyTextureToScreen.png");
		}
	}

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(renderTexture);
	LLGI::SafeRelease(renderPass);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);
}

void test_multiRenderPass(LLGI::DeviceType deviceType)
{
	auto compiler = LLGI::CreateCompiler(deviceType);

	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("MRT", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(pp, window.get()));

	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());
	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	std::array<LLGI::CommandList*, 3> commandLists;
	for (size_t i = 0; i < commandLists.size(); i++)
		commandLists[i] = graphics->CreateCommandList(sfMemoryPool);

	LLGI::TextureInitializationParameter texParam;
	texParam.Size = LLGI::Vec2I(256, 256);

	LLGI::RenderTextureInitializationParameter renderTexParam;
	renderTexParam.Size = LLGI::Vec2I(256, 256);

	auto renderTexture1 = graphics->CreateRenderTexture(renderTexParam);
	auto renderTexture2 = graphics->CreateRenderTexture(renderTexParam);
	LLGI::Texture* renderTextures[2] = {renderTexture1, renderTexture2};
	auto renderPass = graphics->CreateRenderPass(renderTextures, 2, nullptr);
	assert(renderPass->GetRenderTextureCount() == 2);

	auto texture = graphics->CreateTexture(texParam);

	auto texture_buf = (LLGI::Color8*)texture->Lock();
	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			texture_buf[x + y * 256].R = 255;
			texture_buf[x + y * 256].G = 255;
			texture_buf[x + y * 256].B = 255;
			texture_buf[x + y * 256].A = 255;
		}
	}
	texture->Unlock();

	std::shared_ptr<LLGI::Shader> shader_paste_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_paste_ps = nullptr;

	TestHelper::CreateShader(
		graphics.get(), deviceType, "simple_texture_rectangle.vert", "simple_texture_rectangle.frag", shader_paste_vs, shader_paste_ps);

	std::shared_ptr<LLGI::Shader> shader_mrt_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_mrt_ps = nullptr;

	TestHelper::CreateShader(
		graphics.get(), deviceType, "simple_texture_rectangle.vert", "simple_mrt_texture_rectangle.frag", shader_mrt_vs, shader_mrt_ps);

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics.get(),
								LLGI::Vec3F(-0.75, -0.25, 0.5),
								LLGI::Vec3F(-0.25, -0.75, 0.5),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb,
								ib);

	std::shared_ptr<LLGI::VertexBuffer> vb2;
	std::shared_ptr<LLGI::IndexBuffer> ib2;
	TestHelper::CreateRectangle(graphics.get(),
								LLGI::Vec3F(0.25, 0.75, 0.5),
								LLGI::Vec3F(0.75, 0.25, 0.5),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(0, 255, 0, 255),
								vb2,
								ib2);

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pips;

	while (count < 60)
	{
		if (!platform->NewFrame())
		{
			break;
		}

		sfMemoryPool->NewFrame();

		LLGI::Color8 color1;
		color1.R = 0;
		color1.G = count % 255;
		color1.B = 0;
		color1.A = 255;
		renderPass->SetIsColorCleared(true);
		renderPass->SetClearColor(color1);

		LLGI::Color8 color2;
		color2.R = count % 255;
		color2.G = 0;
		color2.B = 0;
		color2.A = 255;

		auto commandList = commandLists[count % commandLists.size()];
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());

		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass));

		auto renderPassSc = platform->GetCurrentScreen(color2, true);
		auto renderPassPipelineStateSc = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPassSc));

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

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_mrt_vs.get());
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_mrt_ps.get());
			pip->SetRenderPassPipelineState(renderPassPipelineState.get());
			pip->Compile();

			pips[renderPassPipelineState] = LLGI::CreateSharedPtr(pip);
		}

		// Render to RenderTargetTexture
		commandList->SetPipelineState(pips[renderPassPipelineState].get());
		commandList->SetTexture(
			texture, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);
		commandList->EndRenderPass();

		commandList->BeginRenderPass(platform->GetCurrentScreen(color2, true));
		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());

		if (pips.count(renderPassPipelineStateSc) == 0)
		{
			auto pip = graphics->CreatePiplineState();
			pip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			pip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			pip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			pip->VertexLayoutNames[0] = "POSITION";
			pip->VertexLayoutNames[1] = "UV";
			pip->VertexLayoutNames[2] = "COLOR";
			pip->VertexLayoutCount = 3;

			pip->SetShader(LLGI::ShaderStageType::Vertex, shader_paste_vs.get());
			pip->SetShader(LLGI::ShaderStageType::Pixel, shader_paste_ps.get());
			pip->SetRenderPassPipelineState(renderPassPipelineStateSc.get());
			pip->Compile();

			pips[renderPassPipelineStateSc] = LLGI::CreateSharedPtr(pip);
		}

		// Render to Backbuffer
		commandList->SetPipelineState(pips[renderPassPipelineStateSc].get());

		commandList->SetVertexBuffer(vb.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib.get());
		commandList->SetTexture(
			renderTexture1, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->SetVertexBuffer(vb2.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib2.get());
		commandList->SetTexture(
			renderTexture2, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
		commandList->Draw(2);

		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto screenTexture = platform->GetCurrentScreen(LLGI::Color8(), true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(screenTexture);
			Bitmap2D(data, screenTexture->GetSizeAs2D().X, screenTexture->GetSizeAs2D().Y, screenTexture->GetFormat())
				.Save("RenderPass.MRT.png");
		}
	}

	pips.clear();

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	LLGI::SafeRelease(renderTexture1);
	LLGI::SafeRelease(renderTexture2);
	LLGI::SafeRelease(renderPass);
	LLGI::SafeRelease(texture);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);

	LLGI::SafeRelease(compiler);
}

TestRegister RenderPass_Basic("RenderPass.Basic",
							  [](LLGI::DeviceType device) -> void { test_renderPass(device, RenderPassTestMode::None); });

TestRegister RenderPass_MSAA("RenderPass.MSAA", [](LLGI::DeviceType device) -> void { test_renderPass(device, RenderPassTestMode::MSAA); });

TestRegister RenderPass_MSAADepth("RenderPass.MSAADepth",
								  [](LLGI::DeviceType device) -> void { test_renderPass(device, RenderPassTestMode::MSAADepth); });

TestRegister RenderPass_CopyTexture("RenderPass.CopyTexture",
									[](LLGI::DeviceType device) -> void { test_renderPass(device, RenderPassTestMode::CopyTexture); });

TestRegister RenderPass_CopyTextureToScreen("RenderPass.CopyTextureToScreen",
											[](LLGI::DeviceType device) -> void { test_copyTextureToScreen(device); });

TestRegister RenderPass_MRT("RenderPass.MRT", [](LLGI::DeviceType device) -> void { test_multiRenderPass(device); });
