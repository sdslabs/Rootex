
#include "LLGI.IndexBufferDX12.h"
#include "../LLGI.IndexBuffer.h"

namespace LLGI
{

IndexBufferDX12::IndexBufferDX12() {}

IndexBufferDX12::~IndexBufferDX12() { SafeRelease(indexBuffer_); }

bool IndexBufferDX12::Initialize(GraphicsDX12* graphics, int32_t stride, int32_t count)
{
	indexBuffer_ = graphics->CreateResource(D3D12_HEAP_TYPE_UPLOAD,
											DXGI_FORMAT_UNKNOWN,
											D3D12_RESOURCE_DIMENSION_BUFFER,
											D3D12_RESOURCE_STATE_GENERIC_READ,
											Vec2I(stride * count, 1));
	stride_ = stride;
	count_ = count;
	return (indexBuffer_ != nullptr);
}

void* IndexBufferDX12::Lock()
{
	auto hr = indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_));
	return SUCCEEDED(hr) ? mapped_ : nullptr;
}

void* IndexBufferDX12::Lock(int32_t offset, int32_t size)
{
	auto hr = indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_));
	return SUCCEEDED(hr) ? mapped_ + offset : nullptr;
}

void IndexBufferDX12::Unlock()
{
	indexBuffer_->Unmap(0, nullptr);
	mapped_ = nullptr;
}

} // namespace LLGI
