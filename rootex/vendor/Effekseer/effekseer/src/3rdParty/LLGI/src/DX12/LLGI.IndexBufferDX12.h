
#pragma once

#include "../LLGI.IndexBuffer.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

using namespace DirectX;

namespace LLGI
{

class IndexBufferDX12 : public IndexBuffer
{
private:
	ID3D12Resource* indexBuffer_ = nullptr;
	uint8_t* mapped_ = nullptr;
	int32_t stride_ = 0;
	int32_t count_ = 0;

public:
	bool Initialize(GraphicsDX12* graphics, int32_t stride, int32_t count);

	IndexBufferDX12();
	~IndexBufferDX12() override;

	virtual void* Lock();
	virtual void* Lock(int32_t offset, int32_t size);
	virtual void Unlock();
	virtual int32_t GetStride() { return stride_; }
	virtual int32_t GetCount() { return count_; }
	ID3D12Resource* Get() { return indexBuffer_; }
};

} // namespace LLGI
