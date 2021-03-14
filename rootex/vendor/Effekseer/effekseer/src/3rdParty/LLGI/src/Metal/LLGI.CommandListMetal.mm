#include "LLGI.CommandListMetal.h"
#include "LLGI.ConstantBufferMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.IndexBufferMetal.h"
#include "LLGI.Metal_Impl.h"
#include "LLGI.PipelineStateMetal.h"
#include "LLGI.RenderPassMetal.h"
#include "LLGI.TextureMetal.h"
#include "LLGI.VertexBufferMetal.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

CommandList_Impl::CommandList_Impl() {}

CommandList_Impl::~CommandList_Impl()
{
	if (commandBuffer != nullptr)
	{
		[commandBuffer release];
		commandBuffer = nullptr;
	}

	if (renderEncoder != nullptr)
	{
		[renderEncoder release];
	}

	if (fence != nullptr)
	{
		[fence release];
	}
}

bool CommandList_Impl::Initialize(Graphics_Impl* graphics)
{
	graphics_ = graphics;
	fence = [graphics->device newFence];
	return true;
}

void CommandList_Impl::Begin()
{
	if (commandBuffer != nullptr)
	{
		[commandBuffer release];
		commandBuffer = nullptr;
	}

	commandBuffer = [graphics_->commandQueue commandBuffer];
	[commandBuffer retain];

	auto t = this;

	[commandBuffer addCompletedHandler:^(id buffer) {
	  t->isCompleted = true;
	}];
}

void CommandList_Impl::End() {}

void CommandList_Impl::BeginRenderPass(RenderPass_Impl* renderPass)
{
	for (size_t i = 0; i < renderPass->pixelFormats.size(); i++)
	{
		if (renderPass->isColorCleared)
		{
			auto r_ = renderPass->clearColor.R / 255.0;
			auto g_ = renderPass->clearColor.G / 255.0;
			auto b_ = renderPass->clearColor.B / 255.0;
			auto a_ = renderPass->clearColor.A / 255.0;

			renderPass->renderPassDescriptor.colorAttachments[i].loadAction = MTLLoadActionClear;
			renderPass->renderPassDescriptor.colorAttachments[i].clearColor = MTLClearColorMake(r_, g_, b_, a_);
		}
		else
		{
			renderPass->renderPassDescriptor.colorAttachments[i].loadAction = MTLLoadActionDontCare;
		}
	}

	if (renderPass->isDepthCleared)
	{
		renderPass->renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
		renderPass->renderPassDescriptor.depthAttachment.clearDepth = 1.0;

		if (renderPass->depthStencilFormat != MTLPixelFormatDepth32Float_Stencil8
#if TARGET_OS_MACOS
			&& renderPass->depthStencilFormat != MTLPixelFormatDepth24Unorm_Stencil8
#endif
		)
		{
			renderPass->renderPassDescriptor.stencilAttachment.loadAction = MTLLoadActionClear;
			renderPass->renderPassDescriptor.stencilAttachment.clearStencil = 0;
		}
	}
	else
	{
		renderPass->renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionDontCare;
		renderPass->renderPassDescriptor.stencilAttachment.loadAction = MTLLoadActionDontCare;
	}

	renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPass->renderPassDescriptor];

	[renderEncoder waitForFence:fence beforeStages:MTLRenderStageVertex];
}

void CommandList_Impl::EndRenderPass()
{
	if (renderEncoder)
	{
		[renderEncoder updateFence:fence afterStages:MTLRenderStageFragment];
		[renderEncoder endEncoding];
		renderEncoder = nullptr;
	}
}

void CommandList_Impl::BeginRenderPassWithPlatform(id<MTLRenderCommandEncoder> renderEncoder)
{
	this->renderEncoder = renderEncoder;

	if (this->renderEncoder)
	{
		[this->renderEncoder retain];
		// TODO : make correct. wait can do only once per encorder
		// [this->renderEncoder waitForFence:fence beforeStages:MTLRenderStageVertex];
	}
}

void CommandList_Impl::EndRenderPassWithPlatform()
{
	if (renderEncoder)
	{
		// TODO : make correct. wait can do only once per encorder
		// [renderEncoder updateFence:fence afterStages:MTLRenderStageFragment];
		[renderEncoder release];
		renderEncoder = nullptr;
	}
}

void CommandList_Impl::SetScissor(int32_t x, int32_t y, int32_t width, int32_t height)
{
	MTLScissorRect rect;
	rect.x = x;
	rect.y = y;
	rect.width = width;
	rect.height = height;
	[renderEncoder setScissorRect:rect];
}

void CommandList_Impl::SetVertexBuffer(Buffer_Impl* vertexBuffer, int32_t stride, int32_t offset)
{
	[renderEncoder setVertexBuffer:vertexBuffer->buffer offset:offset atIndex:VertexBufferIndex];
}

CommandListMetal::CommandListMetal() { impl = new CommandList_Impl(); }

CommandListMetal::~CommandListMetal()
{
	if (isInRenderPass_)
	{
		EndRenderPass();
	}

	if (isInBegin_)
	{
		End();
	}

	WaitUntilCompleted();

	for (int w = 0; w < 2; w++)
	{
		for (int f = 0; f < 2; f++)
		{
			for (int m = 0; m < 2; m++)
			{
				[samplers[w][f][m] release];
				[samplerStates[w][f][m] release];
			}
		}
	}

	SafeDelete(impl);
	SafeRelease(graphics_);
}

bool CommandListMetal::Initialize(Graphics* graphics)
{
	SafeAddRef(graphics);
	SafeRelease(graphics_);
	graphics_ = graphics;

	auto graphics_metal_ = static_cast<GraphicsMetal*>(graphics);

	// Sampler
	for (int w = 0; w < 2; w++)
	{
		for (int f = 0; f < 2; f++)
		{
			for (int m = 0; m < 3; m++)
			{
				MTLSamplerAddressMode ws[2];
				ws[0] = MTLSamplerAddressModeClampToEdge;
				ws[1] = MTLSamplerAddressModeRepeat;

				MTLSamplerMinMagFilter fsmin[2];
				fsmin[0] = MTLSamplerMinMagFilterNearest;
				fsmin[1] = MTLSamplerMinMagFilterLinear;

				MTLSamplerMipFilter msmip[3];
				msmip[0] = MTLSamplerMipFilterNotMipmapped;
				msmip[1] = MTLSamplerMipFilterLinear;
				msmip[2] = MTLSamplerMipFilterNearest;

				MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
				samplerDescriptor.minFilter = fsmin[f];
				samplerDescriptor.magFilter = fsmin[f];
				samplerDescriptor.mipFilter = msmip[m];
				samplerDescriptor.sAddressMode = ws[w];
				samplerDescriptor.tAddressMode = ws[w];

				samplers[w][f][m] = samplerDescriptor;
				samplerStates[w][f][m] = [graphics_metal_->GetImpl()->device newSamplerStateWithDescriptor:samplerDescriptor];
			}
		}
	}

	return impl->Initialize(graphics_metal_->GetImpl());
}

void CommandListMetal::Begin()
{
	impl->Begin();

	CommandList::Begin();
}

void CommandListMetal::End()
{
	impl->End();
	CommandList::End();
}

void CommandListMetal::SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) { impl->SetScissor(x, y, width, height); }

void CommandListMetal::Draw(int32_t primitiveCount, int32_t instanceCount)
{
	BindingVertexBuffer vb_;
	BindingIndexBuffer ib_;
	PipelineState* pip_ = nullptr;

	const int mipmapFilter = 1;

	bool isVBDirtied = false;
	bool isIBDirtied = false;
	bool isPipDirtied = false;

	GetCurrentVertexBuffer(vb_, isVBDirtied);
	GetCurrentIndexBuffer(ib_, isIBDirtied);
	GetCurrentPipelineState(pip_, isPipDirtied);

	assert(vb_.vertexBuffer != nullptr);
	assert(ib_.indexBuffer != nullptr);
	assert(pip_ != nullptr);

	auto vb = static_cast<VertexBufferMetal*>(vb_.vertexBuffer);
	auto ib = static_cast<IndexBufferMetal*>(ib_.indexBuffer);
	auto pip = static_cast<PipelineStateMetal*>(pip_);

	// set cull mode
	if (pip->Culling == LLGI::CullingMode::Clockwise)
	{
		[impl->renderEncoder setCullMode:MTLCullModeFront];
	}
	else if (pip->Culling == LLGI::CullingMode::CounterClockwise)
	{
		[impl->renderEncoder setCullMode:MTLCullModeBack];
	}
	else
	{
		[impl->renderEncoder setCullMode:MTLCullModeNone];
	}

	[impl->renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];

	if (isVBDirtied)
	{
		impl->SetVertexBuffer(vb->GetImpl(), vb_.stride, vb_.offset);
	}

	// assign constant buffer
	ConstantBuffer* vcb = nullptr;
	GetCurrentConstantBuffer(ShaderStageType::Vertex, vcb);
	if (vcb != nullptr)
	{
		auto vcb_ = static_cast<ConstantBufferMetal*>(vcb);
		[impl->renderEncoder setVertexBuffer:vcb_->GetImpl()->buffer offset:vcb_->GetOffset() atIndex:0];
	}

	ConstantBuffer* pcb = nullptr;
	GetCurrentConstantBuffer(ShaderStageType::Pixel, pcb);
	if (pcb != nullptr)
	{
		auto pcb_ = static_cast<ConstantBufferMetal*>(pcb);
		[impl->renderEncoder setFragmentBuffer:pcb_->GetImpl()->buffer offset:pcb_->GetOffset() atIndex:0];
	}

	// Assign textures
	for (int stage_ind = 0; stage_ind < (int32_t)ShaderStageType::Max; stage_ind++)
	{
		for (int unit_ind = 0; unit_ind < currentTextures[stage_ind].size(); unit_ind++)
		{
			if (currentTextures[stage_ind][unit_ind].texture == nullptr)
				continue;

			auto texture = (TextureMetal*)currentTextures[stage_ind][unit_ind].texture;
			auto wm = (int32_t)currentTextures[stage_ind][unit_ind].wrapMode;
			auto mm = (int32_t)currentTextures[stage_ind][unit_ind].minMagFilter;
			auto pm = 0;
			if (texture->GetImpl()->texture.mipmapLevelCount >= 2)
			{
				pm = mipmapFilter;
			}

			if (stage_ind == (int32_t)ShaderStageType::Vertex)
			{
				[impl->renderEncoder setVertexTexture:texture->GetImpl()->texture atIndex:unit_ind];
				[impl->renderEncoder setVertexSamplerState:samplerStates[wm][mm][pm] atIndex:unit_ind];
			}

			if (stage_ind == (int32_t)ShaderStageType::Pixel)
			{
				[impl->renderEncoder setFragmentTexture:texture->GetImpl()->texture atIndex:unit_ind];
				[impl->renderEncoder setFragmentSamplerState:samplerStates[wm][mm][pm] atIndex:unit_ind];
			}
		}
	}

	if (isPipDirtied)
	{
		[impl->renderEncoder setRenderPipelineState:pip->GetImpl()->pipelineState];

		if (pip->GetImpl()->depthStencilState != nullptr)
		{
			[impl->renderEncoder setDepthStencilState:pip->GetImpl()->depthStencilState];
		}

		[impl->renderEncoder setStencilReferenceValue:pip->StencilRef];
	}

	// draw
	MTLPrimitiveType topology = MTLPrimitiveTypeTriangle;
	MTLIndexType indexType = MTLIndexTypeUInt32;
	int indexPerPrim = 0;

	if (pip_->Topology == TopologyType::Triangle)
	{
		indexPerPrim = 3;
		topology = MTLPrimitiveTypeTriangle;
	}
	else if (pip_->Topology == TopologyType::Line)
	{
		indexPerPrim = 2;
		topology = MTLPrimitiveTypeLine;
	}
	else if (pip_->Topology == TopologyType::Point)
	{
		indexPerPrim = 1;
		topology = MTLPrimitiveTypePoint;
	}
	else
	{
		assert(0);
	}

	if (ib->GetStride() == 2)
	{
		indexType = MTLIndexTypeUInt16;
	}

	[impl->renderEncoder drawIndexedPrimitives:topology
									indexCount:primitiveCount * indexPerPrim
									 indexType:indexType
								   indexBuffer:ib->GetImpl()->buffer
							 indexBufferOffset:ib_.offset
								 instanceCount:instanceCount];

	CommandList::Draw(primitiveCount, instanceCount);
}

void CommandListMetal::CopyTexture(Texture* src, Texture* dst)
{
	if (isInRenderPass_)
	{
		Log(LogType::Error, "Please call CopyTexture outside of RenderPass");
		return;
	}

	auto srcTex = static_cast<TextureMetal*>(src);
	auto dstTex = static_cast<TextureMetal*>(dst);

	id<MTLBlitCommandEncoder> blitEncoder = [impl->commandBuffer blitCommandEncoder];

	auto regionSize = srcTex->GetSizeAs2D();

	MTLRegion region = {{0, 0, 0}, {(uint32_t)regionSize.X, (uint32_t)regionSize.Y, 1}};

	[blitEncoder copyFromTexture:srcTex->GetImpl()->texture
					 sourceSlice:0
					 sourceLevel:0
					sourceOrigin:region.origin
					  sourceSize:region.size
					   toTexture:dstTex->GetImpl()->texture
				destinationSlice:0
				destinationLevel:0
			   destinationOrigin:{0, 0, 0}];
	[blitEncoder endEncoding];

	RegisterReferencedObject(src);
	RegisterReferencedObject(dst);
}

void CommandListMetal::GenerateMipMap(Texture* src)
{
	auto srcTex = static_cast<TextureMetal*>(src);

	id<MTLBlitCommandEncoder> blitEncoder = [impl->commandBuffer blitCommandEncoder];
	[blitEncoder generateMipmapsForTexture:srcTex->GetImpl()->texture];
	[blitEncoder endEncoding];
}

void CommandListMetal::BeginRenderPass(RenderPass* renderPass)
{
	auto renderPass_ = static_cast<RenderPassMetal*>(renderPass)->GetImpl();
	impl->BeginRenderPass(renderPass_);

	CommandList::BeginRenderPass(renderPass);
}

void CommandListMetal::EndRenderPass()
{
	impl->EndRenderPass();
	CommandList::EndRenderPass();
}

void CommandListMetal::WaitUntilCompleted()
{
	if (impl->commandBuffer != nullptr)
	{
		auto status = [impl->commandBuffer status];
		if (status == MTLCommandBufferStatusNotEnqueued)
		{
			return;
		}

		[impl->commandBuffer waitUntilCompleted];
	}
}

bool CommandListMetal::BeginWithPlatform(void* platformContextPtr) { return CommandList::BeginWithPlatform(platformContextPtr); }

void CommandListMetal::EndWithPlatform() { CommandList::EndWithPlatform(); }

bool CommandListMetal::BeginRenderPassWithPlatformPtr(void* platformPtr)
{
	auto pp = reinterpret_cast<CommandListMetalPlatformRenderPassContext*>(platformPtr);

	impl->BeginRenderPassWithPlatform(pp->RenderEncoder);
	return CommandList::BeginRenderPassWithPlatformPtr(platformPtr);
}

bool CommandListMetal::EndRenderPassWithPlatformPtr()
{
	impl->EndRenderPassWithPlatform();
	return CommandList::EndRenderPassWithPlatformPtr();
}

CommandList_Impl* CommandListMetal::GetImpl() { return impl; }

}
