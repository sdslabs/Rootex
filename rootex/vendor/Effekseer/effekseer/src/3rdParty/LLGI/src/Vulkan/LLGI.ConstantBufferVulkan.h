
#pragma once

#include "../LLGI.ConstantBuffer.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.GraphicsVulkan.h"

namespace LLGI
{
class SingleFrameMemoryPoolVulkan;

class ConstantBufferVulkan : public ConstantBuffer
{
private:
	std::shared_ptr<GraphicsVulkan> graphics_;
	std::unique_ptr<Buffer> buffer_;
	int memSize_ = 0;
	void* data = nullptr;
	int32_t offset_ = 0;

public:
	ConstantBufferVulkan();
	~ConstantBufferVulkan() override;

	bool Initialize(GraphicsVulkan* graphics, int32_t size);
	bool InitializeAsShortTime(GraphicsVulkan* graphics, SingleFrameMemoryPoolVulkan* memoryPool, int32_t size);

	void* Lock() override;
	void* Lock(int32_t offset, int32_t size) override;
	void Unlock() override;
	int32_t GetSize() override;
	int32_t GetOffset() const { return offset_; }
	vk::Buffer GetBuffer() { return buffer_->buffer(); }
};

} // namespace LLGI
