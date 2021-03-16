#include "LLGI.RenderPassMetal.h"
#include "LLGI.CommandListMetal.h"
#include "LLGI.ConstantBufferMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.IndexBufferMetal.h"
#include "LLGI.Metal_Impl.h"
#include "LLGI.PipelineStateMetal.h"
#include "LLGI.ShaderMetal.h"
#include "LLGI.SingleFrameMemoryPoolMetal.h"
#include "LLGI.TextureMetal.h"
#include "LLGI.VertexBufferMetal.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

void RenderPassPipelineState_Impl::SetKey(RenderPassPipelineStateKey key)
{
	pixelFormats.resize(key.RenderTargetFormats.size());

	for (size_t i = 0; i < pixelFormats.size(); i++)
	{
		pixelFormats.at(i) = ConvertFormat(key.RenderTargetFormats.at(i));
	}

	depthStencilFormat = ConvertFormat(key.DepthFormat);
}

RenderPass_Impl::RenderPass_Impl() {}

RenderPass_Impl::~RenderPass_Impl()
{
	if (renderPassDescriptor != nullptr)
	{
		[renderPassDescriptor release];
		renderPassDescriptor = nullptr;
	}
}

bool RenderPass_Impl::Initialize()
{
	renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
	return true;
}

void RenderPass_Impl::UpdateTarget(Texture_Impl** textures,
								   int32_t textureCount,
								   Texture_Impl* depthTexture,
								   Texture_Impl* resolvedTexture,
								   Texture_Impl* resolvedDepthTexture)
{
	pixelFormats.resize(textureCount);

	for (int i = 0; i < textureCount; i++)
	{
		renderPassDescriptor.colorAttachments[i].texture = textures[i]->texture;
		pixelFormats.at(i) = textures[i]->texture.pixelFormat;

		if (resolvedTexture != nullptr)
		{
			renderPassDescriptor.colorAttachments[i].resolveTexture = resolvedTexture->texture;
			renderPassDescriptor.colorAttachments[i].storeAction = MTLStoreActionMultisampleResolve;
		}
	}

	if (depthTexture != nullptr)
	{
		renderPassDescriptor.depthAttachment.texture = depthTexture->texture;

		if (resolvedDepthTexture != nullptr)
		{
			renderPassDescriptor.depthAttachment.resolveTexture = resolvedDepthTexture->texture;
			renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionMultisampleResolve;
		}

		if (HasStencil(ConvertFormat(depthTexture->texture.pixelFormat)))
		{
			renderPassDescriptor.stencilAttachment.texture = depthTexture->texture;

			if (resolvedDepthTexture != nullptr)
			{
				renderPassDescriptor.stencilAttachment.resolveTexture = resolvedDepthTexture->texture;
				renderPassDescriptor.stencilAttachment.storeAction = MTLStoreActionMultisampleResolve;
			}
		}

		depthStencilFormat = depthTexture->texture.pixelFormat;
	}
}

RenderPassMetal::RenderPassMetal()
{
	impl = new RenderPass_Impl();
	impl->Initialize();
}

RenderPassMetal::~RenderPassMetal() { SafeDelete(impl); }

bool RenderPassMetal::UpdateRenderTarget(
	Texture** textures, int32_t textureCount, Texture* depthTexture, Texture* resolvedTexture, Texture* resolvedDepthTexture)
{
	if (!assignRenderTextures(textures, textureCount))
	{
		return false;
	}

	if (!assignDepthTexture(depthTexture))
	{
		return false;
	}

	if (!assignResolvedRenderTexture(resolvedTexture))
	{
		return false;
	}

	if (!assignResolvedDepthTexture(resolvedDepthTexture))
	{
		return false;
	}

	if (!getSize(screenSize_, (const Texture**)textures, textureCount, depthTexture))
	{
		return false;
	}

	std::array<Texture_Impl*, RenderTargetMax> texturesImpl;
	texturesImpl.fill(nullptr);
	Texture_Impl* depthTextureImpl = nullptr;

	for (int32_t i = 0; i < textureCount; i++)
	{
		if (textures[i] == nullptr)
			continue;

		texturesImpl.at(i) = reinterpret_cast<TextureMetal*>(textures[i])->GetImpl();
	}

	if (depthTexture != nullptr)
	{
		depthTextureImpl = reinterpret_cast<const TextureMetal*>(depthTexture)->GetImpl();
	}

	Texture_Impl* resolvedTextureImpl = nullptr;
	Texture_Impl* resolvedDepthTextureImpl = nullptr;

	if (resolvedTexture != nullptr)
	{
		resolvedTextureImpl = reinterpret_cast<TextureMetal*>(resolvedTexture)->GetImpl();
	}

	if (resolvedDepthTexture != nullptr)
	{
		resolvedDepthTextureImpl = reinterpret_cast<TextureMetal*>(resolvedDepthTexture)->GetImpl();
	}

	impl->UpdateTarget(texturesImpl.data(), textureCount, depthTextureImpl, resolvedTextureImpl, resolvedDepthTextureImpl);

	return true;
}

void RenderPassMetal::SetIsColorCleared(bool isColorCleared)
{
	impl->isColorCleared = isColorCleared;
	RenderPass::SetIsColorCleared(isColorCleared);
}

void RenderPassMetal::SetIsDepthCleared(bool isDepthCleared)
{
	impl->isDepthCleared = isDepthCleared;
	RenderPass::SetIsDepthCleared(isDepthCleared);
}

void RenderPassMetal::SetClearColor(const Color8& color)
{
	impl->clearColor = color;
	RenderPass::SetClearColor(color);
}

RenderPass_Impl* RenderPassMetal::GetImpl() const { return impl; }

RenderPassPipelineStateMetal::RenderPassPipelineStateMetal() { impl = new RenderPassPipelineState_Impl(); }

RenderPassPipelineStateMetal::~RenderPassPipelineStateMetal() { SafeDelete(impl); }

void RenderPassPipelineStateMetal::SetKey(const RenderPassPipelineStateKey& key)
{
	Key = key;
	impl->SetKey(key);
}

RenderPassPipelineState_Impl* RenderPassPipelineStateMetal::GetImpl() const { return impl; }

}
