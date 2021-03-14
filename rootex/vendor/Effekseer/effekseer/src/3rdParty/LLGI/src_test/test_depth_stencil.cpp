#include "TestHelper.h"
#include "test.h"

#include <fstream>
#include <iostream>
#include <map>

enum class DepthStencilTestMode
{
	Depth,
	Stencil,
	DepthAsTexture,
};

void test_depth_stencil(LLGI::DeviceType deviceType, DepthStencilTestMode mode)
{
	if (mode == DepthStencilTestMode::DepthAsTexture)
	{
		if (deviceType != LLGI::DeviceType::DirectX12 && deviceType != LLGI::DeviceType::Default)
			return;
	}

	auto compiler = LLGI::CreateSharedPtr(LLGI::CreateCompiler(LLGI::DeviceType::Default));

	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("DepthStencil", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(pp, window.get()));

	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());
	auto sfMemoryPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));
	auto commandList = LLGI::CreateSharedPtr(graphics->CreateCommandList(sfMemoryPool.get()));

	std::shared_ptr<LLGI::Shader> shader_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_ps = nullptr;

	TestHelper::CreateShader(graphics.get(), deviceType, "simple_rectangle.vert", "simple_rectangle.frag", shader_vs, shader_ps);

	std::shared_ptr<LLGI::Shader> shader_screen_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_screen_ps = nullptr;

	TestHelper::CreateShader(
		graphics.get(), deviceType, "simple_texture_rectangle.vert", "simple_texture_rectangle.frag", shader_screen_vs, shader_screen_ps);

	// Green: near
	std::shared_ptr<LLGI::VertexBuffer> vb1;
	std::shared_ptr<LLGI::IndexBuffer> ib1;
	TestHelper::CreateRectangle(graphics.get(),
								LLGI::Vec3F(-0.5f, 0.5f, 0.5f),
								LLGI::Vec3F(0.5f, -0.5f, 0.5f),
								LLGI::Color8(0, 255, 0, 255),
								LLGI::Color8(),
								vb1,
								ib1);

	// Blue: far
	std::shared_ptr<LLGI::VertexBuffer> vb2;
	std::shared_ptr<LLGI::IndexBuffer> ib2;
	TestHelper::CreateRectangle(graphics.get(),
								LLGI::Vec3F(-0.2f, 0.2f, 0.8f),
								LLGI::Vec3F(0.7f, -0.7f, 0.8f),
								LLGI::Color8(0, 0, 255, 255),
								LLGI::Color8(),
								vb2,
								ib2);

	// Screen
	std::shared_ptr<LLGI::VertexBuffer> vb3;
	std::shared_ptr<LLGI::IndexBuffer> ib3;
	TestHelper::CreateRectangle(graphics.get(),
								LLGI::Vec3F(-1.0f, 1.0f, 0.5f),
								LLGI::Vec3F(1.0f, -1.0f, 0.5f),
								LLGI::Color8(255, 255, 255, 255),
								LLGI::Color8(255, 255, 255, 255),
								vb3,
								ib3);

	struct PipelineStateSet
	{
		std::shared_ptr<LLGI::PipelineState> writeState; // write depth or stencil
		std::shared_ptr<LLGI::PipelineState> testState;	 // depth-test or stencil-test
	};
	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, PipelineStateSet> pips;

	std::map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> screenPips;

	std::shared_ptr<LLGI::Texture> colorBuffer;
	std::shared_ptr<LLGI::Texture> depthBuffer;
	std::shared_ptr<LLGI::RenderPass> renderPass;

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

		auto screenRenderPass = platform->GetCurrentScreen(color, true, true);

		if (colorBuffer == nullptr)
		{
			LLGI::RenderTextureInitializationParameter param;
			param.Size = screenRenderPass->GetRenderTexture(0)->GetSizeAs2D();
			colorBuffer = LLGI::CreateSharedPtr(graphics->CreateRenderTexture(param));

			LLGI::DepthTextureInitializationParameter depthParam;
			depthParam.Size = screenRenderPass->GetRenderTexture(0)->GetSizeAs2D();

			if (mode == DepthStencilTestMode::Stencil)
			{
				depthParam.Mode = LLGI::DepthTextureMode::DepthStencil;
			}

			depthBuffer = LLGI::CreateSharedPtr(graphics->CreateDepthTexture(depthParam));

			std::array<LLGI::Texture*, 1> colorBuffers;
			colorBuffers[0] = colorBuffer.get();

			renderPass = LLGI::CreateSharedPtr(
				graphics->CreateRenderPass(colorBuffers.data(), static_cast<int32_t>(colorBuffers.size()), depthBuffer.get()));
		}

		renderPass->SetIsColorCleared(true);
		renderPass->SetClearColor(color);
		renderPass->SetIsDepthCleared(true);

		auto renderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(renderPass.get()));

		if (pips.count(renderPassPipelineState) == 0)
		{
			auto writepip = graphics->CreatePiplineState();
			writepip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			writepip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			writepip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			writepip->VertexLayoutNames[0] = "POSITION";
			writepip->VertexLayoutNames[1] = "UV";
			writepip->VertexLayoutNames[2] = "COLOR";
			writepip->VertexLayoutCount = 3;

			writepip->IsBlendEnabled = false;

			if (mode == DepthStencilTestMode::Depth || mode == DepthStencilTestMode::DepthAsTexture)
			{
				writepip->IsDepthWriteEnabled = true;
			}

			if (mode == DepthStencilTestMode::Stencil)
			{
				writepip->IsStencilTestEnabled = true;
				writepip->StencilFailOp = LLGI::StencilOperatorType::Keep;
				writepip->StencilDepthFailOp = LLGI::StencilOperatorType::Replace;
				writepip->StencilPassOp = LLGI::StencilOperatorType::Replace;
				writepip->StencilCompareFunc = LLGI::CompareFuncType::Always;
			}

			writepip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs.get());
			writepip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps.get());
			writepip->SetRenderPassPipelineState(renderPassPipelineState.get());
			writepip->Compile();

			auto testpip = graphics->CreatePiplineState();
			testpip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			testpip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			testpip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			testpip->VertexLayoutNames[0] = "POSITION";
			testpip->VertexLayoutNames[1] = "UV";
			testpip->VertexLayoutNames[2] = "COLOR";
			testpip->VertexLayoutCount = 3;

			testpip->IsBlendEnabled = false;

			if (mode == DepthStencilTestMode::Depth || mode == DepthStencilTestMode::DepthAsTexture)
			{
				testpip->IsDepthTestEnabled = true;
				testpip->DepthFunc = LLGI::DepthFuncType::Always;
			}

			if (mode == DepthStencilTestMode::Stencil)
			{
				testpip->IsStencilTestEnabled = true;
				testpip->StencilRef = 0x7f;
				testpip->StencilFailOp = LLGI::StencilOperatorType::Keep;
				testpip->StencilDepthFailOp = LLGI::StencilOperatorType::Keep;
				testpip->StencilPassOp = LLGI::StencilOperatorType::Keep;
				testpip->StencilCompareFunc = LLGI::CompareFuncType::Greater;
				testpip->StencilReadMask = 0xff;
				testpip->StencilWriteMask = 0xff;
			}

			testpip->SetShader(LLGI::ShaderStageType::Vertex, shader_vs.get());
			testpip->SetShader(LLGI::ShaderStageType::Pixel, shader_ps.get());
			testpip->SetRenderPassPipelineState(renderPassPipelineState.get());
			testpip->Compile();

			pips[renderPassPipelineState].writeState = LLGI::CreateSharedPtr(writepip);
			pips[renderPassPipelineState].testState = LLGI::CreateSharedPtr(testpip);
		}

		auto screenRenderPassPipelineState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(screenRenderPass));
		if (screenPips.count(screenRenderPassPipelineState) == 0)
		{
			auto screenpip = graphics->CreatePiplineState();
			screenpip->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			screenpip->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			screenpip->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			screenpip->VertexLayoutNames[0] = "POSITION";
			screenpip->VertexLayoutNames[1] = "UV";
			screenpip->VertexLayoutNames[2] = "COLOR";
			screenpip->VertexLayoutCount = 3;

			screenpip->IsBlendEnabled = false;
			screenpip->SetShader(LLGI::ShaderStageType::Vertex, shader_screen_vs.get());
			screenpip->SetShader(LLGI::ShaderStageType::Pixel, shader_screen_ps.get());
			screenpip->SetRenderPassPipelineState(screenRenderPassPipelineState.get());
			screenpip->Compile();
			screenPips[screenRenderPassPipelineState] = LLGI::CreateSharedPtr(screenpip);
		}

		commandList->Begin();
		commandList->BeginRenderPass(renderPass.get());

		// First, green rectangle.
		commandList->SetVertexBuffer(vb1.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib1.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].writeState.get());
		commandList->Draw(2);

		// Next, blue rectangle.
		commandList->SetVertexBuffer(vb2.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib2.get());
		commandList->SetPipelineState(pips[renderPassPipelineState].testState.get());
		commandList->Draw(2);

		commandList->EndRenderPass();

		// Screen
		commandList->BeginRenderPass(screenRenderPass);
		commandList->SetVertexBuffer(vb3.get(), sizeof(SimpleVertex), 0);
		commandList->SetIndexBuffer(ib3.get());

		if (mode == DepthStencilTestMode::DepthAsTexture)
		{
			commandList->SetTexture(
				depthBuffer.get(), LLGI::TextureWrapMode::Clamp, LLGI::TextureMinMagFilter::Linear, 0, LLGI::ShaderStageType::Pixel);
		}
		else
		{
			commandList->SetTexture(
				colorBuffer.get(), LLGI::TextureWrapMode::Clamp, LLGI::TextureMinMagFilter::Linear, 0, LLGI::ShaderStageType::Pixel);
		}

		commandList->SetPipelineState(screenPips[screenRenderPassPipelineState].get());
		commandList->Draw(2);
		commandList->EndRenderPass();

		commandList->End();

		graphics->Execute(commandList.get());

		platform->Present();
		count++;

		if (TestHelper::GetIsCaptureRequired() && count == 30)
		{
			commandList->WaitUntilCompleted();
			auto texture = platform->GetCurrentScreen(color, true)->GetRenderTexture(0);
			auto data = graphics->CaptureRenderTarget(texture);

			// save
			if (mode == DepthStencilTestMode::Depth)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save("DepthStencil.Depth.png");
			}
			else if (mode == DepthStencilTestMode::Stencil)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save("DepthStencil.Stencil.png");
			}
			else if (mode == DepthStencilTestMode::DepthAsTexture)
			{
				Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat())
					.Save("DepthStencil.DepthAsTexture.png");
			}
		}
	}

	pips.clear();
	screenPips.clear();

	graphics->WaitFinish();
}

TestRegister DepthStencil_Depth("DepthStencil.Depth",
								[](LLGI::DeviceType device) -> void { test_depth_stencil(device, DepthStencilTestMode::Depth); });

TestRegister DepthStencil_Stencil("DepthStencil.Stencil",
								  [](LLGI::DeviceType device) -> void { test_depth_stencil(device, DepthStencilTestMode::Stencil); });

TestRegister DepthStencil_DepthAsTexture("DepthStencil.DepthAsTexture", [](LLGI::DeviceType device) -> void {
	test_depth_stencil(device, DepthStencilTestMode::DepthAsTexture);
});
