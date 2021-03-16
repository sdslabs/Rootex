
#pragma once

#include "../LLGI.VertexBuffer.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

namespace LLGI
{

class VertexBufferDX12 : public VertexBuffer
{
private:
	ID3D12Resource* vertexBuffer_ = nullptr;
	uint8_t* mapped_ = nullptr;
	int32_t size_ = 0;

public:
	bool Initialize(GraphicsDX12* graphics, int32_t size);

	VertexBufferDX12();
	~VertexBufferDX12() override;

	virtual void* Lock();
	virtual void* Lock(int32_t offset, int32_t size);
	virtual void Unlock();
	virtual int32_t GetSize();
	ID3D12Resource* Get() { return vertexBuffer_; }
};

} // namespace LLGI
