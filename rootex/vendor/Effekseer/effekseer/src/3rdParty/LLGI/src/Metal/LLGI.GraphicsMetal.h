#pragma once

#include "../LLGI.Graphics.h"
#include "../Utils/LLGI.FixedSizeVector.h"
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

struct GraphicsView
{
	id<CAMetalDrawable> drawable;
};

class GraphicsMetal : public Graphics
{
	Graphics_Impl* impl = nullptr;

	//! cached
	std::unordered_map<RenderPassPipelineStateKey, std::shared_ptr<RenderPassPipelineStateMetal>, RenderPassPipelineStateKey::Hash>
		renderPassPipelineStates_;

	std::shared_ptr<RenderPassMetal> renderPass_ = nullptr;
	std::function<GraphicsView()> getGraphicsView_;
	std::vector<CommandList*> executingCommandList_;

public:
	GraphicsMetal();
	~GraphicsMetal() override;

	bool Initialize(std::function<GraphicsView()> getGraphicsView);

	void SetWindowSize(const Vec2I& windowSize) override;

	void Execute(CommandList* commandList) override;

	void WaitFinish() override;

	VertexBuffer* CreateVertexBuffer(int32_t size) override;

	IndexBuffer* CreateIndexBuffer(int32_t stride, int32_t count) override;

	Shader* CreateShader(DataStructure* data, int32_t count) override;

	PipelineState* CreatePiplineState() override;

	SingleFrameMemoryPool* CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount) override;

	CommandList* CreateCommandList(SingleFrameMemoryPool* memoryPool) override;

	ConstantBuffer* CreateConstantBuffer(int32_t size) override;

	RenderPass* CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture) override;

	RenderPass* CreateRenderPass(Texture* texture, Texture* resolvedTexture, Texture* depthTexture, Texture* resolvedDepthTexture) override;

	Texture* CreateTexture(const TextureInitializationParameter& parameter) override;

	Texture* CreateRenderTexture(const RenderTextureInitializationParameter& parameter) override;

	Texture* CreateDepthTexture(const DepthTextureInitializationParameter& parameter) override;

	Texture* CreateTexture(uint64_t texid) override;

	RenderPassPipelineState* CreateRenderPassPipelineState(RenderPass* renderPass) override;

	RenderPassPipelineState* CreateRenderPassPipelineState(const RenderPassPipelineStateKey& key) override;

	std::vector<uint8_t> CaptureRenderTarget(Texture* renderTarget) override;

	Graphics_Impl* GetImpl() const;

	RenderPassMetal* GetRenderPass() const { return renderPass_.get(); }
};

} // namespace LLGI
