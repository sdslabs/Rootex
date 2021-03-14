#include "LLGI.TextureMetal.h"
#include "LLGI.Metal_Impl.h"
#include <TargetConditionals.h>

namespace LLGI
{

Texture_Impl::Texture_Impl() {}

Texture_Impl::~Texture_Impl()
{
	if (texture != nullptr)
	{
		[texture release];
		texture = nullptr;
	}
}

bool Texture_Impl::Initialize(id<MTLDevice> device, const Vec2I& size, TextureFormatType format, int samplingCount, TextureType type, int MipMapCount)
{
	MTLTextureDescriptor* textureDescriptor = nullptr;

	bool isMipmapped = MipMapCount >= 2;

	if (type == TextureType::Depth)
	{
		textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:ConvertFormat(format)
																			   width:size.X
																			  height:size.Y
																		   mipmapped:isMipmapped];
		textureDescriptor.usage = MTLTextureUsageRenderTarget;
		textureDescriptor.textureType = MTLTextureType2D;
		textureDescriptor.storageMode = MTLStorageModePrivate;
		textureDescriptor.sampleCount = samplingCount;

		if (samplingCount > 1)
		{
			textureDescriptor.textureType = MTLTextureType2DMultisample;
			textureDescriptor.storageMode = MTLStorageModePrivate;
		}
	}
	else
	{
		textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:ConvertFormat(format)
																			   width:size.X
																			  height:size.Y
																		   mipmapped:isMipmapped];
	}

	if (isMipmapped)
	{
		textureDescriptor.mipmapLevelCount = MipMapCount;
	}

	texture = [device newTextureWithDescriptor:textureDescriptor];

	size_ = size;

	fromExternal_ = false;

	return true;
}

bool Texture_Impl::Initialize(Graphics_Impl* graphics, const RenderTextureInitializationParameter& parameter)
{
	id<MTLDevice> device = graphics->device;
	MTLTextureDescriptor* textureDescriptor = nullptr;

	textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:ConvertFormat(parameter.Format)
																		   width:parameter.Size.X
																		  height:parameter.Size.Y
																	   mipmapped:NO];
	textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
	textureDescriptor.depth = 1;

	if (parameter.SamplingCount > 1)
	{
		textureDescriptor.textureType = MTLTextureType2DMultisample;
		textureDescriptor.storageMode = MTLStorageModePrivate;
	}
	else
	{
		textureDescriptor.textureType = MTLTextureType2D;
		// Make copy enabled in GetBuffer
		// TODO : Optimize
#if !(TARGET_OS_IPHONE) && !(TARGET_OS_SIMULATOR)
		textureDescriptor.storageMode = MTLStorageModeManaged;
#else
		textureDescriptor.storageMode = MTLStorageModePrivate;
#endif
	}

	textureDescriptor.sampleCount = parameter.SamplingCount;

	texture = [device newTextureWithDescriptor:textureDescriptor];

	size_ = parameter.Size;

	fromExternal_ = false;

	return true;
}

void Texture_Impl::Reset(id<MTLTexture> nativeTexture)
{
	if (nativeTexture != nullptr)
	{
		[nativeTexture retain];
	}

	if (texture != nullptr)
	{
		[texture release];
	}

	texture = nativeTexture;

	if (texture != nullptr)
	{
		size_.X = static_cast<int32_t>(texture.width);
		size_.Y = static_cast<int32_t>(texture.height);
	}
	else
	{
		size_.X = 0.0f;
		size_.Y = 0.0f;
	}

	fromExternal_ = true;
}

void Texture_Impl::Write(const uint8_t* data)
{
	MTLRegion region = {{0, 0, 0}, {static_cast<uint32_t>(size_.X), static_cast<uint32_t>(size_.Y), 1}};

	auto allSize = GetTextureMemorySize(ConvertFormat(texture.pixelFormat), size_);

	[texture replaceRegion:region mipmapLevel:0 withBytes:data bytesPerRow:allSize / size_.Y];
}

TextureMetal::TextureMetal() { impl = new Texture_Impl(); }

TextureMetal::~TextureMetal()
{
	SafeDelete(impl);
	SafeRelease(owner_);
}

bool TextureMetal::Initialize(GraphicsMetal* owner, const TextureInitializationParameter& parameter)
{
	type_ = TextureType::Color;

	SafeAssign(owner_, static_cast<ReferenceObject*>(owner));

	if (!impl->Initialize(owner->GetImpl()->device, parameter.Size, parameter.Format, 1, type_, parameter.MipMapCount))
	{
		return false;
	}

	format_ = ConvertFormat(impl->texture.pixelFormat);
	data.resize(GetTextureMemorySize(format_, impl->size_));

	return true;
}

bool TextureMetal::Initialize(GraphicsMetal* owner, const RenderTextureInitializationParameter& parameter)
{
	type_ = TextureType::Render;
	samplingCount_ = parameter.SamplingCount;

	SafeAssign(owner_, static_cast<ReferenceObject*>(owner));

	if (!impl->Initialize(owner->GetImpl(), parameter))
	{
		return false;
	}

	format_ = ConvertFormat(impl->texture.pixelFormat);
	data.resize(GetTextureMemorySize(format_, impl->size_));

	return true;
}

bool TextureMetal::Initialize(GraphicsMetal* owner, const DepthTextureInitializationParameter& parameter)
{
	type_ = TextureType::Depth;

	/*
	 #if TARGET_OS_IOS
			 auto format = MTLPixelFormatDepth32Float_Stencil8;
	 #elif TARGET_OS_MAC
			 bool supported = device.isDepth24Stencil8PixelFormatSupported;

			 auto format = (supported) ? MTLPixelFormatDepth24Unorm_Stencil8 : MTLPixelFormatDepth32Float_Stencil8;
	 #endif
	 **/

	samplingCount_ = parameter.SamplingCount;

	SafeAssign(owner_, static_cast<ReferenceObject*>(owner));

	TextureFormatType format = TextureFormatType::D32;
	if (parameter.Mode == DepthTextureMode::DepthStencil)
	{
		format = TextureFormatType::D24S8;

		if (!owner->GetImpl()->device.isDepth24Stencil8PixelFormatSupported)
		{
			return false;
		}
	}

	if (!impl->Initialize(owner->GetImpl()->device, parameter.Size, format, parameter.SamplingCount, type_, 1))
	{
		return false;
	}

	format_ = ConvertFormat(impl->texture.pixelFormat);
	data.resize(GetTextureMemorySize(format_, impl->size_));

	return true;
}

bool TextureMetal::Initialize(GraphicsMetal* owner, id<MTLTexture> externalTexture)
{
	if (externalTexture == nullptr)
	{
		return false;
	}

	type_ = TextureType::Color;
	impl->Reset(externalTexture);

	format_ = ConvertFormat(impl->texture.pixelFormat);

	return true;
}

void TextureMetal::Reset(id<MTLTexture> nativeTexture)
{
	type_ = TextureType::Screen;
	impl->Reset(nativeTexture);

	format_ = ConvertFormat(impl->texture.pixelFormat);
}

void* TextureMetal::Lock() { return data.data(); }

void TextureMetal::Unlock() { impl->Write(data.data()); }

Vec2I TextureMetal::GetSizeAs2D() const { return impl->size_; }

Texture_Impl* TextureMetal::GetImpl() const { return impl; }

}
