
#pragma once

#include "../LLGI.IndexBuffer.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.GraphicsVulkan.h"

namespace LLGI
{

class IndexBufferVulkan : public IndexBuffer
{
private:
	std::shared_ptr<GraphicsVulkan> graphics_;
	std::unique_ptr<Buffer> cpuBuf;
	std::unique_ptr<Buffer> gpuBuf;
	void* data = nullptr;
	int32_t memSize = 0;
	int32_t count_ = 0;
	int32_t stride_ = 0;

public:
	bool Initialize(GraphicsVulkan* graphics, int32_t stride, int32_t count);

	IndexBufferVulkan();
	~IndexBufferVulkan() override;

	void* Lock() override;
	void* Lock(int32_t offset, int32_t size) override;
	void Unlock() override;
	int32_t GetStride() override;
	int32_t GetCount() override;

	vk::Buffer GetBuffer() { return gpuBuf->buffer(); }
};

} // namespace LLGI
