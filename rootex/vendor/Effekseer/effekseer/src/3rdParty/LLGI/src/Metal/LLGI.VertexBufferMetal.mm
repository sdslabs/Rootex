#include "LLGI.VertexBufferMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.Metal_Impl.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

VertexBufferMetal::VertexBufferMetal() { impl = new Buffer_Impl(); }

VertexBufferMetal::~VertexBufferMetal() { SafeDelete(impl); }

bool VertexBufferMetal::Initialize(Graphics* graphics, int32_t size)
{
	auto graphics_ = static_cast<GraphicsMetal*>(graphics);
	return impl->Initialize(graphics_->GetImpl(), size);
}

void* VertexBufferMetal::Lock() { return impl->GetBuffer(); }

void* VertexBufferMetal::Lock(int32_t offset, int32_t size)
{
	NSCAssert(0 <= offset && offset + size <= impl->size_, @"Run off the buffer");

	auto buffer_ = static_cast<uint8_t*>(impl->GetBuffer());
	buffer_ += offset;
	return buffer_;
}

void VertexBufferMetal::Unlock() {}

int32_t VertexBufferMetal::GetSize() { return impl->size_; }

}
