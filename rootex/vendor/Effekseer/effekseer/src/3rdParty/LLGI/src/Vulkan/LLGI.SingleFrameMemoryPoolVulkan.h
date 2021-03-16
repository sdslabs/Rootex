
#pragma once

#include "LLGI.BaseVulkan.h"
#include "LLGI.GraphicsVulkan.h"

namespace LLGI
{
class GraphicsVulkan;

class InternalSingleFrameMemoryPoolVulkan
{
private:
	int32_t constantBufferSize_ = 0;
	int32_t constantBufferOffset_ = 0;
	VkDevice nativeDevice_ = VK_NULL_HANDLE;
	VkBuffer nativeBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory nativeBufferMemory_ = VK_NULL_HANDLE;

public:
	InternalSingleFrameMemoryPoolVulkan();
	virtual ~InternalSingleFrameMemoryPoolVulkan();
	bool Initialize(GraphicsVulkan* graphics, int32_t constantBufferPoolSize, int32_t drawingCount);
	void Dispose();
	bool GetConstantBuffer(int32_t size, VkBuffer* outResource, VkDeviceMemory* deviceMemory, int32_t* outOffset);
	void Reset();
};

class SingleFrameMemoryPoolVulkan : public SingleFrameMemoryPool
{
private:
	GraphicsVulkan* graphics_ = nullptr;
	bool isStrongRef_ = false;
	std::vector<std::shared_ptr<InternalSingleFrameMemoryPoolVulkan>> memoryPools;
	int32_t currentSwap_ = 0;
	int32_t drawingCount_ = 0;

protected:
	ConstantBuffer* CreateConstantBufferInternal(int32_t size) override;

	ConstantBuffer* ReinitializeConstantBuffer(ConstantBuffer* cb, int32_t size) override;

public:
	SingleFrameMemoryPoolVulkan(
		GraphicsVulkan* graphics, bool isStrongRef, int32_t swapBufferCount, int32_t constantBufferPoolSize, int32_t drawingCount);
	~SingleFrameMemoryPoolVulkan() override;

	bool GetConstantBuffer(int32_t size, VkBuffer* outResource, VkDeviceMemory* deviceMemory, int32_t* outOffset);

	InternalSingleFrameMemoryPoolVulkan* GetInternal();

	int32_t GetDrawingCount() const;

	void NewFrame() override;
};

} // namespace LLGI
