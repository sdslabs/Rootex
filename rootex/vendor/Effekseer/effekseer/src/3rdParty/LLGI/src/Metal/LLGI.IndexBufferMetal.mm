#include "LLGI.IndexBufferMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.Metal_Impl.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

IndexBufferMetal::IndexBufferMetal() { impl = new Buffer_Impl(); }

IndexBufferMetal::~IndexBufferMetal() { SafeDelete(impl); }

bool IndexBufferMetal::Initialize(Graphics* graphics, int32_t stride, int32_t count)
{
	auto graphics_ = static_cast<GraphicsMetal*>(graphics);
	stride_ = stride;
	count_ = count;
	return impl->Initialize(graphics_->GetImpl(), stride * count);
}

void* IndexBufferMetal::Lock() { return impl->GetBuffer(); }

void* IndexBufferMetal::Lock(int32_t offset, int32_t size)
{
	NSCAssert(0 <= offset && offset + size <= impl->size_, @"Run off the buffer");

	auto buffer_ = static_cast<uint8_t*>(impl->GetBuffer());
	buffer_ += offset;
	return buffer_;
}

void IndexBufferMetal::Unlock() {}

int32_t IndexBufferMetal::GetStride() { return stride_; }

int32_t IndexBufferMetal::GetCount() { return count_; }

}
