
#pragma once

#include "../LLGI.Texture.h"
#include "LLGI.GraphicsMetal.h"

namespace LLGI
{

struct Texture_Impl;

class TextureMetal : public Texture
{
private:
	ReferenceObject* owner_ = nullptr;
	Texture_Impl* impl = nullptr;
	std::vector<uint8_t> data;

public:
	TextureMetal();
	~TextureMetal() override;

	bool Initialize(GraphicsMetal* owner, const TextureInitializationParameter& parameter);
	bool Initialize(GraphicsMetal* owner, const RenderTextureInitializationParameter& parameter);
	bool Initialize(GraphicsMetal* owner, const DepthTextureInitializationParameter& parameter);
	bool Initialize(GraphicsMetal* owner, id<MTLTexture> externalTexture);
	void Reset(id<MTLTexture> nativeTexture);
	void* Lock() override;
	void Unlock() override;
	Vec2I GetSizeAs2D() const override;

	Texture_Impl* GetImpl() const;
};

} // namespace LLGI
