#pragma once

#include "renderer/texture.h"
#include "resource_file.h"

/// Representation of an image file which is loaded into a cube texture. 
/// Supports only certain DDS formats. Use texassemble.exe (in DirectXTex) 
/// to assemble cube sides in a suitable format.
class ImageCubeResourceFile : public ResourceFile
{
	Ptr<TextureCube> m_ImageTexture;

	explicit ImageCubeResourceFile(const FilePath& path);

	friend class ResourceLoader;

public:
	explicit ImageCubeResourceFile(const ImageCubeResourceFile&) = delete;
	explicit ImageCubeResourceFile(const ImageCubeResourceFile&&) = delete;
	~ImageCubeResourceFile() = default;

	void reimport() override;

	const TextureCube* getTexture() const { return m_ImageTexture.get(); }
};
