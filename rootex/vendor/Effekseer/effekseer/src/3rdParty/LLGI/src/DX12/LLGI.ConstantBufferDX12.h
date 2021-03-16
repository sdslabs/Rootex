
#pragma once

#include "../LLGI.ConstantBuffer.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

using namespace DirectX;

namespace LLGI
{

class ConstantBufferDX12 : public ConstantBuffer
{
private:
	ID3D12Resource* constantBuffer_ = nullptr;
	int memSize_ = 0;
	int actualSize_ = 0;
	int offset_ = 0;
	uint8_t* mapped_ = nullptr;

public:
	bool Initialize(GraphicsDX12* graphics, int32_t size);

	bool InitializeAsShortTime(SingleFrameMemoryPoolDX12* memoryPool, int32_t size);

	ConstantBufferDX12();
	~ConstantBufferDX12() override;

	void* Lock() override;
	void* Lock(int32_t offset, int32_t size) override;
	void Unlock() override;
	int32_t GetSize() override;

	/**
		@brief	memory size must be multiple of 256 in DirectX12. So specified size and actual size are different.
	*/
	int32_t GetActualSize() const;
	int32_t GetOffset() const;

	ID3D12Resource* Get() { return constantBuffer_; }
};

} // namespace LLGI
