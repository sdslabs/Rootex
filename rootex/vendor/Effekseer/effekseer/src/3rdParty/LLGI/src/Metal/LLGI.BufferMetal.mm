#include "LLGI.BufferMetal.h"
#include "LLGI.CommandListMetal.h"
#include "LLGI.ConstantBufferMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.IndexBufferMetal.h"
#include "LLGI.Metal_Impl.h"
#include "LLGI.PipelineStateMetal.h"
#include "LLGI.RenderPassMetal.h"
#include "LLGI.ShaderMetal.h"
#include "LLGI.SingleFrameMemoryPoolMetal.h"
#include "LLGI.TextureMetal.h"
#include "LLGI.VertexBufferMetal.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

Buffer_Impl::Buffer_Impl() {}

Buffer_Impl::~Buffer_Impl()
{
	if (buffer != nullptr)
	{
		[buffer release];
		buffer = nullptr;
	}
}

bool Buffer_Impl::Initialize(Graphics_Impl* graphics, int32_t size)
{
	buffer = [graphics->device newBufferWithLength:size options:MTLResourceOptionCPUCacheModeDefault];

	size_ = size;

	return true;
}

void* Buffer_Impl::GetBuffer() { return buffer.contents; }

BufferMetal::BufferMetal() { impl = new Buffer_Impl(); }

BufferMetal::~BufferMetal() { SafeDelete(impl); }

bool BufferMetal::Initialize(Graphics* graphics, int32_t size)
{
	auto graphics_ = static_cast<GraphicsMetal*>(graphics);
	return impl->Initialize(graphics_->GetImpl(), size);
}

void* BufferMetal::GetBuffer()
{
	assert(impl != nullptr);
	return impl->GetBuffer();
}

Buffer_Impl* BufferMetal::GetImpl() const { return impl; }
}
