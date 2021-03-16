#include "TestHelper.h"
#include "test.h"
#include <Utils/LLGI.CommandListPool.h>
#include <array>

void test_capture_texture(LLGI::DeviceType deviceType)
{
	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("Capture", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(pp, window.get()));
	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());

	auto sfMemoryPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));

	auto commandListPool = std::make_shared<LLGI::CommandListPool>(graphics.get(), sfMemoryPool.get(), 3);

	auto compareTexture = [graphics](bool isRenderTexture, LLGI::Vec2I size, LLGI::TextureFormatType format) -> void {
		std::shared_ptr<LLGI::Texture> texture;
		if (isRenderTexture)
		{
			LLGI::RenderTextureInitializationParameter texParam;
			texParam.Size = size;
			texParam.Format = format;
			texture = LLGI::CreateSharedPtr(graphics->CreateRenderTexture(texParam));
		}
		else
		{
			LLGI::TextureInitializationParameter texParam;
			texParam.Size = size;
			texParam.Format = format;
			texture = LLGI::CreateSharedPtr(graphics->CreateTexture(texParam));
		}

		TestHelper::WriteDummyTexture(texture.get());
		auto src = TestHelper::CreateDummyTextureData(texture->GetSizeAs2D(), texture->GetFormat());
		auto dst = graphics->CaptureRenderTarget(texture.get());

		if (src != dst)
		{
			std::cout << "Not equal" << std::endl;
		}
	};

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

		if (count == 10)
		{
			compareTexture(true, LLGI::Vec2I(256, 256), LLGI::TextureFormatType::R8G8B8A8_UNORM);
			compareTexture(true, LLGI::Vec2I(256, 256), LLGI::TextureFormatType::R32G32B32A32_FLOAT);
			compareTexture(true, LLGI::Vec2I(256, 256), LLGI::TextureFormatType::R8_UNORM);
			compareTexture(true, LLGI::Vec2I(255, 255), LLGI::TextureFormatType::R8G8B8A8_UNORM);
			compareTexture(true, LLGI::Vec2I(255, 255), LLGI::TextureFormatType::R32G32B32A32_FLOAT);
			compareTexture(true, LLGI::Vec2I(255, 255), LLGI::TextureFormatType::R8_UNORM);

			compareTexture(false, LLGI::Vec2I(256, 256), LLGI::TextureFormatType::R8G8B8A8_UNORM);
			compareTexture(false, LLGI::Vec2I(256, 256), LLGI::TextureFormatType::R32G32B32A32_FLOAT);
			compareTexture(false, LLGI::Vec2I(256, 256), LLGI::TextureFormatType::R8_UNORM);
			compareTexture(false, LLGI::Vec2I(255, 255), LLGI::TextureFormatType::R8G8B8A8_UNORM);
			compareTexture(false, LLGI::Vec2I(255, 255), LLGI::TextureFormatType::R32G32B32A32_FLOAT);
			compareTexture(false, LLGI::Vec2I(255, 255), LLGI::TextureFormatType::R8_UNORM);
		}

		auto renderPass = platform->GetCurrentScreen(color, true, false);

		auto commandList = commandListPool->Get();
		commandList->Begin();
		commandList->BeginRenderPass(renderPass);

		commandList->EndRenderPass();
		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;
	}
}

void test_capture(LLGI::DeviceType deviceType, LLGI::Vec2I windowSize)
{
	int count = 0;

	LLGI::PlatformParameter pp;
	pp.Device = deviceType;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("Capture", windowSize));
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(pp, window.get()));
	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());

	auto sfMemoryPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));

	auto commandListPool = std::make_shared<LLGI::CommandListPool>(graphics.get(), sfMemoryPool.get(), 3);

	std::shared_ptr<LLGI::Shader> shader_vs = nullptr;
	std::shared_ptr<LLGI::Shader> shader_ps = nullptr;

	TestHelper::CreateShader(graphics.get(), deviceType, "simple_rectangle.vert", "simple_rectangle.frag", shader_vs, shader_ps);

	std::shared_ptr<LLGI::VertexBuffer> vb;
	std::shared_ptr<LLGI::IndexBuffer> ib;
	TestHelper::CreateRectangle(graphics.get(),
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

			// save
			std::string path = "Capture.Size" + std::to_string(windowSize.X) + ".png";
			Bitmap2D(data, texture->GetSizeAs2D().X, texture->GetSizeAs2D().Y, texture->GetFormat()).Save(path.c_str());
		}
	}

	pips.clear();
}

#if defined(__linux__) || defined(__APPLE__) || defined(WIN32)

TestRegister Capture_Size1279("Capture.Size1279", [](LLGI::DeviceType device) -> void { test_capture(device, LLGI::Vec2I(1279, 719)); });

TestRegister Capture_Size800("Capture.Size800", [](LLGI::DeviceType device) -> void { test_capture(device, LLGI::Vec2I(800, 600)); });

TestRegister Capture_Size1280("Capture.Size1280", [](LLGI::DeviceType device) -> void { test_capture(device, LLGI::Vec2I(1280, 720)); });

TestRegister Capture_Texture("Capture.Texture", [](LLGI::DeviceType device) -> void { test_capture_texture(device); });

#endif
