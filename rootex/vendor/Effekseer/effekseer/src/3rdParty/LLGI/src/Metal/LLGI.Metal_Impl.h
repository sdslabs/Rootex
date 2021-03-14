#pragma once

#include "../LLGI.Graphics.h"
#include "../LLGI.PipelineState.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

//! which buffer is used as vertex buffer
const int VertexBufferIndex = 2;

struct CommandList_Impl;
struct Buffer_Impl;
struct Texture_Impl;

MTLPixelFormat ConvertFormat(TextureFormatType format);

TextureFormatType ConvertFormat(MTLPixelFormat format);

struct Graphics_Impl
{
	id<MTLDevice> device = nullptr;
	id<MTLCommandQueue> commandQueue = nullptr;
	id<CAMetalDrawable> drawable;
	int maxMultiSamplingCount = 0;

	Graphics_Impl();
	virtual ~Graphics_Impl();
	bool Initialize();
	void Execute(CommandList_Impl* commandBuffer);
};

struct RenderPass_Impl
{
	MTLRenderPassDescriptor* renderPassDescriptor;

	Color8 clearColor;
	bool isColorCleared;
	bool isDepthCleared;
	FixedSizeVector<MTLPixelFormat, RenderTargetMax> pixelFormats;
	MTLPixelFormat depthStencilFormat = MTLPixelFormatInvalid;

	RenderPass_Impl();
	~RenderPass_Impl();
	bool Initialize();

	void UpdateTarget(Texture_Impl** textures,
					  int32_t textureCount,
					  Texture_Impl* depthTexture,
					  Texture_Impl* resolvedTexture,
					  Texture_Impl* resolvedDepthTexture);
};

struct RenderPassPipelineState_Impl
{
	FixedSizeVector<MTLPixelFormat, RenderTargetMax> pixelFormats;
	MTLPixelFormat depthStencilFormat = MTLPixelFormatInvalid;

	void SetKey(RenderPassPipelineStateKey key);
};

struct CommandList_Impl
{
	Graphics_Impl* graphics_ = nullptr;
	id<MTLCommandBuffer> commandBuffer = nullptr;
	id<MTLRenderCommandEncoder> renderEncoder = nullptr;
	id<MTLFence> fence = nullptr;

	bool isCompleted = true;

	CommandList_Impl();
	~CommandList_Impl();

	bool Initialize(Graphics_Impl* graphics);

	void Begin();
	void End();
	void BeginRenderPass(RenderPass_Impl* renderPass);
	void EndRenderPass();
	void BeginRenderPassWithPlatform(id<MTLRenderCommandEncoder> renderEncoder);
	void EndRenderPassWithPlatform();

	void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height);
	void SetVertexBuffer(Buffer_Impl* vertexBuffer, int32_t stride, int32_t offset);
};

struct Shader_Impl
{
	id<MTLLibrary> library = nullptr;

	Shader_Impl();
	~Shader_Impl();
	bool Initialize(Graphics_Impl* graphics, const void* data, int size);
};

struct PipelineState_Impl
{
	id<MTLRenderPipelineState> pipelineState;
	id<MTLDepthStencilState> depthStencilState = nullptr;
	MTLRenderPipelineDescriptor* pipelineStateDescriptor = nullptr;

	PipelineState_Impl();
	~PipelineState_Impl();

	bool Compile(PipelineState* self, Graphics_Impl* graphics);
};

struct Buffer_Impl
{
	id<MTLBuffer> buffer = nullptr;
	int32_t size_;

	Buffer_Impl();
	virtual ~Buffer_Impl();

	bool Initialize(Graphics_Impl* graphics, int32_t size);

	void* GetBuffer();
};

struct Texture_Impl
{
	id<MTLTexture> texture = nullptr;
	Vec2I size_;
	bool fromExternal_ = false;

	Texture_Impl();
	virtual ~Texture_Impl();

	bool Initialize(id<MTLDevice> device, const Vec2I& size, TextureFormatType format, int samplingCount, TextureType type, int MipMapCount);
	bool Initialize(Graphics_Impl* graphics, const RenderTextureInitializationParameter& parameter);
	void Reset(id<MTLTexture> nativeTexture); // for wrap swapchain backbuffer.

	void Write(const uint8_t* data);
};

} // namespace LLGI
