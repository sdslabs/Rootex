#pragma once

#include "../LLGI.CommandList.h"
#include "LLGI.BaseVulkan.h"

namespace LLGI
{

class DescriptorPoolVulkan
{
private:
	std::shared_ptr<GraphicsVulkan> graphics_;
	vk::DescriptorPool descriptorPool_ = nullptr;
	int32_t offset = 0;
	std::vector<std::vector<vk::DescriptorSet>> cache;

public:
	DescriptorPoolVulkan(std::shared_ptr<GraphicsVulkan> graphics, int32_t size, int stage);
	virtual ~DescriptorPoolVulkan();
	const std::vector<vk::DescriptorSet>& Get(PipelineStateVulkan* pip);
	void Reset();
};

struct PlatformContextVulkan
{
	VkCommandBuffer commandBuffer;
};

class CommandListVulkan : public CommandList
{
private:
	std::shared_ptr<GraphicsVulkan> graphics_;
	vk::CommandBuffer currentCommandBuffer_;
	std::vector<vk::CommandBuffer> commandBuffers_;
	std::vector<std::shared_ptr<DescriptorPoolVulkan>> descriptorPools;
	int32_t currentSwapBufferIndex_;
	std::vector<vk::Fence> fences_;
	vk::Sampler samplers_[2][2];

public:
	CommandListVulkan();
	~CommandListVulkan() override;

	bool Initialize(GraphicsVulkan* graphics, int32_t drawingCount);

	void Begin() override;
	void End() override;

	bool BeginWithPlatform(void* platformContextPtr) override;
	void EndWithPlatform() override;

	void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;
	void Draw(int32_t primitiveCount, int32_t instanceCount) override;
	void CopyTexture(Texture* src, Texture* dst) override;

	void GenerateMipMap(Texture* src) override;

	void BeginRenderPass(RenderPass* renderPass) override;
	void EndRenderPass() override;
	vk::CommandBuffer GetCommandBuffer() const;
	vk::Fence GetFence() const;

	void WaitUntilCompleted() override;
};

} // namespace LLGI
