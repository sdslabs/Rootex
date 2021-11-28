#pragma once

#include "resource_file.h"
#include "renderer/texture.h"

/// Representation of an image file in Direct3D supported texture format. Cannot be used for processing on CPU
/// Supports BMP, JPEG, PNG, TIFF, GIF, HD Photo, or other WIC supported file containers
class ImageResourceFile : public ResourceFile
{
	Ref<GPUTexture> m_GPUImageTexture;
	Ref<CPUTexture> m_CPUImageTexture;
	bool m_IsCPUAccess;

	explicit ImageResourceFile(const FilePath& path, bool cpuAccess = false);

	void loadCPUTexture();

	friend class ResourceLoader;

public:
	explicit ImageResourceFile(const ImageResourceFile&) = delete;
	explicit ImageResourceFile(const ImageResourceFile&&) = delete;
	~ImageResourceFile() = default;

	void reimport() override;

	const Ref<GPUTexture> getGPUTexture() { return m_GPUImageTexture; }
	const Ref<CPUTexture> getCPUTexture() { return m_CPUImageTexture; }

	unsigned int getWidth() const { return m_GPUImageTexture->getWidth(); }
	unsigned int getHeight() const { return m_GPUImageTexture->getHeight(); }

	void setCPUAccess(bool cpuAccess);
	bool isCPUAccess() { return m_IsCPUAccess; };

	void uploadCPUTexturetoGPU();
};
