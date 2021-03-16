#include "LLGI.BufferDX12.h"

namespace LLGI
{

BufferDX12::BufferDX12() {}

BufferDX12::~BufferDX12() { SafeRelease(buffer_); }

bool BufferDX12::Initialize(GraphicsDX12* graphics, const int32_t size)
{
	buffer_ = graphics->CreateResource(
		D3D12_HEAP_TYPE_READBACK, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST, Vec2I(size, 1));

	size_ = size;
	state_ = D3D12_RESOURCE_STATE_COPY_DEST;
	return (buffer_ != nullptr);
}

void* BufferDX12::Lock()
{
	D3D12_RANGE range{};
	range.Begin = 0;
	range.End = size_;
	auto hr = buffer_->Map(0, &range, reinterpret_cast<void**>(&mapped_));
	return SUCCEEDED(hr) ? mapped_ : nullptr;
}

void BufferDX12::Unlock()
{
	buffer_->Unmap(0, nullptr);
	mapped_ = nullptr;
}

int32_t BufferDX12::GetSize() { return size_; }

void BufferDX12::ResourceBarrior(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state)
{
	if (state_ == state)
		return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = buffer_;
	barrier.Transition.StateBefore = state_;
	barrier.Transition.StateAfter = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);
	state_ = state;
}

} // namespace LLGI
