#include "LLGI.SingleFrameMemoryPoolDX12.h"
#include "LLGI.CommandListDX12.h"
#include "LLGI.ConstantBufferDX12.h"

namespace LLGI
{

InternalSingleFrameMemoryPoolDX12::InternalSingleFrameMemoryPoolDX12(GraphicsDX12* graphics,
																	 int32_t constantBufferPoolSize,
																	 int32_t drawingCount)
{
	constantBufferSize_ = (constantBufferPoolSize + 255) & ~255; // buffer size should be multiple of 256
	constantBuffer_ = graphics->CreateResource(D3D12_HEAP_TYPE_UPLOAD,
											   DXGI_FORMAT_UNKNOWN,
											   D3D12_RESOURCE_DIMENSION_BUFFER,
											   D3D12_RESOURCE_STATE_GENERIC_READ,
											   Vec2I(constantBufferSize_, 1));
}

InternalSingleFrameMemoryPoolDX12 ::~InternalSingleFrameMemoryPoolDX12() { SafeRelease(constantBuffer_); }

bool InternalSingleFrameMemoryPoolDX12::GetConstantBuffer(int32_t size, ID3D12Resource*& resource, int32_t& offset)
{
	if (constantBufferOffset_ + size > constantBufferSize_)
		return false;

	resource = constantBuffer_;
	offset = constantBufferOffset_;
	constantBufferOffset_ += size;
	return true;
}
void InternalSingleFrameMemoryPoolDX12::Reset() { constantBufferOffset_ = 0; }

SingleFrameMemoryPoolDX12::SingleFrameMemoryPoolDX12(
	GraphicsDX12* graphics, bool isStrongRef, int32_t swapBufferCount, int32_t constantBufferPoolSize, int32_t drawingCount)
	: SingleFrameMemoryPool(swapBufferCount), graphics_(graphics), isStrongRef_(isStrongRef), drawingCount_(drawingCount)
{
	if (isStrongRef)
	{
		SafeAddRef(graphics_);
	}

	for (int32_t i = 0; i < swapBufferCount; i++)
	{
		auto memoryPool = std::make_shared<InternalSingleFrameMemoryPoolDX12>(graphics, constantBufferPoolSize, drawingCount);
		memoryPools.push_back(memoryPool);
	}

	currentSwap_ = -1;
}

SingleFrameMemoryPoolDX12 ::~SingleFrameMemoryPoolDX12()
{
	memoryPools.clear();

	if (isStrongRef_)
	{
		SafeRelease(graphics_);
	}
}

ConstantBuffer* SingleFrameMemoryPoolDX12::CreateConstantBufferInternal(int32_t size)
{
	auto obj = new ConstantBufferDX12();
	if (!obj->InitializeAsShortTime(this, size))
	{
		SafeRelease(obj);
		return nullptr;
	}

	return obj;
}

ConstantBuffer* SingleFrameMemoryPoolDX12::ReinitializeConstantBuffer(ConstantBuffer* cb, int32_t size)
{
	auto obj = static_cast<ConstantBufferDX12*>(cb);
	if (!obj->InitializeAsShortTime(this, size))
	{
		return nullptr;
	}

	return obj;
}

bool SingleFrameMemoryPoolDX12::GetConstantBuffer(int32_t size, ID3D12Resource*& resource, int32_t& offset)
{
	assert(currentSwap_ >= 0);
	return memoryPools[currentSwap_]->GetConstantBuffer(size, resource, offset);
}

InternalSingleFrameMemoryPoolDX12* SingleFrameMemoryPoolDX12::GetInternal() { return memoryPools[currentSwap_].get(); }

int32_t SingleFrameMemoryPoolDX12::GetDrawingCount() const { return drawingCount_; }

void SingleFrameMemoryPoolDX12::NewFrame()
{
	currentSwap_++;
	currentSwap_ %= memoryPools.size();
	memoryPools[currentSwap_]->Reset();
	SingleFrameMemoryPool::NewFrame();

	assert(currentSwap_ == currentSwapBuffer_);
}

} // namespace LLGI
