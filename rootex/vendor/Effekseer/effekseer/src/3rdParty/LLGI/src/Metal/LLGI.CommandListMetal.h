#pragma once

#include "../LLGI.CommandList.h"
#import <MetalKit/MetalKit.h>

namespace LLGI
{

struct CommandList_Impl;
class IndexBuffer;

struct CommandListMetalPlatformRenderPassContext
{
	id<MTLRenderCommandEncoder> RenderEncoder = nullptr;
};

class CommandListMetal : public CommandList
{
	CommandList_Impl* impl = nullptr;
	Graphics* graphics_ = nullptr;

	MTLSamplerDescriptor* samplers[2][2][3];
	id<MTLSamplerState> samplerStates[2][2][3];

public:
	CommandListMetal();
	~CommandListMetal() override;

	bool Initialize(Graphics* graphics);

	void Begin() override;
	void End() override;
	void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;
	void Draw(int32_t primitiveCount, int32_t instanceCount) override;
	void CopyTexture(Texture* src, Texture* dst) override;
	void GenerateMipMap(Texture* src) override;
	void BeginRenderPass(RenderPass* renderPass) override;
	void EndRenderPass() override;

	void WaitUntilCompleted() override;

	bool BeginWithPlatform(void* platformContextPtr) override;
	void EndWithPlatform() override;

	bool BeginRenderPassWithPlatformPtr(void* platformPtr) override;
	bool EndRenderPassWithPlatformPtr() override;

	CommandList_Impl* GetImpl();
};

} // namespace LLGI
