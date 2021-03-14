#pragma once

#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

namespace LLGI
{
class BufferDX12
{
private:
	ID3D12Resource* buffer_ = nullptr;
	uint8_t* mapped_ = nullptr;
	int32_t size_;
	D3D12_RESOURCE_STATES state_;

public:
	bool Initialize(GraphicsDX12* graphics, const int32_t size);

	BufferDX12();
	virtual ~BufferDX12();

	virtual void* Lock();
	virtual void Unlock();
	virtual int32_t GetSize();
	ID3D12Resource* Get() { return buffer_; }
	void ResourceBarrior(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);
};

} // namespace LLGI
