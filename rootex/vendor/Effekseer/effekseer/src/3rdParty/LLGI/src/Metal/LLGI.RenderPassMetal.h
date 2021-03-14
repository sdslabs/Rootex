#pragma once

#include "../LLGI.Graphics.h"
#import <MetalKit/MetalKit.h>
#include <functional>
#include <unordered_map>

namespace LLGI
{

struct Graphics_Impl;
struct RenderPass_Impl;
struct RenderPassPipelineState_Impl;

class GraphicsMetal;
class RenderPassMetal;
class RenderPassPipelineStateMetal;
class TextureMetal;

class RenderPassMetal : public RenderPass
{
	RenderPass_Impl* impl = nullptr;

public:
	RenderPassMetal();

	~RenderPassMetal() override;

	bool UpdateRenderTarget(
		Texture** textures, int32_t textureCount, Texture* depthTexture, Texture* resolvedTexture, Texture* resolvedDepthTexture);

	void SetIsColorCleared(bool isColorCleared) override;

	void SetIsDepthCleared(bool isDepthCleared) override;

	void SetClearColor(const Color8& color) override;

	RenderPass_Impl* GetImpl() const;
};

class RenderPassPipelineStateMetal : public RenderPassPipelineState
{
private:
	RenderPassPipelineState_Impl* impl = nullptr;

public:
	RenderPassPipelineStateMetal();
	~RenderPassPipelineStateMetal() override;

	void SetKey(const RenderPassPipelineStateKey& key);

	RenderPassPipelineState_Impl* GetImpl() const;
};

} // namespace LLGI
