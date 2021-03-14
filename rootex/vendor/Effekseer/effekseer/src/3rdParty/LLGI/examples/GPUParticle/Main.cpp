
#include <iostream>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../src_test/thirdparty/stb/stb_image.h"

#include <LLGI.CommandList.h>
#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <LLGI.Shader.h>
#include <LLGI.Compiler.h>
#include <LLGI.VertexBuffer.h>
#include <LLGI.IndexBuffer.h>
#include <LLGI.PipelineState.h>
#include <LLGI.Texture.h>
#include "GPUParticle.h"

#ifdef _WIN32
#pragma comment(lib, "d3dcompiler.lib")
#endif



class GPUParticleContext;


namespace LLGI {
	void SetIsGPUDebugEnabled(bool value);
}

// -1.0 ~ 1.0
float randFloat(float minAbs = 0.0f)
{
	float r = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	if (r < minAbs) r = minAbs;
	return r * 2.0f - 1.0f;
}


int main()
{
	int count = 0;

	//LLGI::SetIsGPUDebugEnabled(true);

	LLGI::PlatformParameter pp;
	pp.Device = LLGI::DeviceType::Default;
	//pp.Device = LLGI::DeviceType::Vulkan;
	pp.WaitVSync = true;
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("GPUParticle", LLGI::Vec2I(1280, 720)));
	auto platform = LLGI::CreatePlatform(pp, window.get());

	auto graphics = platform->CreateGraphics();
	auto sfMemoryPool = graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128);

	std::array<LLGI::CommandList*, 3> commandLists;
	for (size_t i = 0; i < commandLists.size(); i++)
		commandLists[i] = graphics->CreateCommandList(sfMemoryPool);




	//GPUParticleRenderPass gpuParticleRenderPass(graphics, pp.Device, platform->GetMaxFrameCount());


	std::shared_ptr<LLGI::Texture> particleTexture;
	{
		int w, h, comp;
		stbi_uc* data = stbi_load(
			EXAMPLE_ASSET_DIR "/Textures/Particle01.png",
			&w, &h, &comp, 4);

		LLGI::TextureInitializationParameter texParam;
		texParam.Size = LLGI::Vec2I(w, h);
		texParam.Format = LLGI::TextureFormatType::R8G8B8A8_UNORM;

		particleTexture = LLGI::CreateSharedPtr(graphics->CreateTexture(texParam));
		auto texture_buf = (LLGI::Color8*)particleTexture->Lock();
		memcpy(texture_buf, data, w * h * comp);
		particleTexture->Unlock();
	}

	GPUParticleShaders shaders;
	if (pp.Device == LLGI::DeviceType::Vulkan) {
		shaders.emitShader = std::make_shared<Shader>(
			graphics, pp.Device,
			EXAMPLE_ASSET_DIR "/Shaders/SPIRV/perticle-emit.vert.spv",
			EXAMPLE_ASSET_DIR "/Shaders/SPIRV/perticle-emit.frag.spv");
		shaders.updateShader = std::make_shared<Shader>(
			graphics, pp.Device,
			EXAMPLE_ASSET_DIR "/Shaders/SPIRV/perticle-update.vert.spv",
			EXAMPLE_ASSET_DIR "/Shaders/SPIRV/perticle-update.frag.spv");
		shaders.renderShader = std::make_shared<Shader>(
			graphics, pp.Device,
			EXAMPLE_ASSET_DIR "/Shaders/SPIRV/perticle-render.vert.spv",
			EXAMPLE_ASSET_DIR "/Shaders/SPIRV/perticle-render.frag.spv");
	}
	else {
		shaders.emitShader = std::make_shared<Shader>(
			graphics, pp.Device,
			EXAMPLE_ASSET_DIR "/Shaders/HLSL_DX12/perticle-emit.vert",
			EXAMPLE_ASSET_DIR "/Shaders/HLSL_DX12/perticle-emit.frag");
		shaders.updateShader = std::make_shared<Shader>(
			graphics, pp.Device,
			EXAMPLE_ASSET_DIR "/Shaders/HLSL_DX12/perticle-update.vert",
			EXAMPLE_ASSET_DIR "/Shaders/HLSL_DX12/perticle-update.frag");
		shaders.renderShader = std::make_shared<Shader>(
			graphics, pp.Device,
			EXAMPLE_ASSET_DIR "/Shaders/HLSL_DX12/perticle-render.vert",
			EXAMPLE_ASSET_DIR "/Shaders/HLSL_DX12/perticle-render.frag");
	}

	auto particleContext = std::make_unique<GPUParticleContext>(
		graphics, pp.Device, platform->GetMaxFrameCount(), 512, particleTexture.get(), shaders);

	//localFront.x = makeRandom(particle, -1.0, 1.0, ParticleRandomSource::Self);
	//localFront.y = makeRandom(particle, -1.0, 1.0, ParticleRandomSource::Self);
	//localFront.z = makeRandom(particle, -1.0, 1.0, ParticleRandomSource::Self);
	//localFront = Vector3::safeNormalize(localFront, Vector3::UnitZ);

	//particleContext->Emit(1, LLGI::Vec3F(0, 0, 0), LLGI::Vec3F(0, 0, 0));
	//particleContext->Emit(1, LLGI::Vec3F(1, 1, 0), LLGI::Vec3F(0.1, 0, 0));
	//particleContext->Emit(1, LLGI::Vec3F(10, 20, 0), LLGI::Vec3F(0, 0, 0));
	//particleContext->Emit(1, LLGI::Vec3F(3, 3, 0), LLGI::Vec3F(0, 0, 0));


	while (true)
	{

		const float radius = 100.0f;
		const float minVelocity = 0.001f;
		const float velocityScale = 0.05f;
		for (int i = 0; i < 100; i++) {
			auto dir = LLGI::Vec3F::Normalize(LLGI::Vec3F(randFloat(), randFloat(), randFloat()));
			auto pos = LLGI::Vec3F(dir.X * radius, dir.Y * radius, dir.Z * radius);
			//auto vel = LLGI::Vec3F(randFloat(minVelocity) * velocityScale, randFloat(minVelocity) * velocityScale, randFloat(minVelocity) * velocityScale);//LLGI::Vec3F(0, 0, 0);//
			auto vel = dir * (randFloat(minVelocity) + 1.0f) * velocityScale;
			particleContext->Emit(5, pos, vel);
		}




		if (!platform->NewFrame())
			break;

		sfMemoryPool->NewFrame();

		particleContext->NewFrame();

		LLGI::Color8 color;
		color.R = 0;
		color.G = 0;
		color.B = 0;
		color.A = 255;


		auto renderPass = platform->GetCurrentScreen(color, true, false); // TODO: isDepthClear is false, because it fails with dx12.

		auto commandList = commandLists[count % commandLists.size()];
		commandList->WaitUntilCompleted();





		commandList->Begin();


		particleContext->Render(renderPass, commandList);

		commandList->End();

		graphics->Execute(commandList);

		platform->Present();
		count++;
		//std::cout << count << std::endl;
	}

	graphics->WaitFinish();

	LLGI::SafeRelease(sfMemoryPool);
	for (size_t i = 0; i < commandLists.size(); i++)
		LLGI::SafeRelease(commandLists[i]);
	LLGI::SafeRelease(graphics);
	LLGI::SafeRelease(platform);

	return 0;
}
