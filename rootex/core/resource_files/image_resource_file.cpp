#include "image_resource_file.h"

ImageResourceFile::ImageResourceFile(const FilePath& path, bool cpuAccess)
    : ResourceFile(Type::Image, path)
    , m_IsCPUAccess(cpuAccess)
{
	reimport();
}

void ImageResourceFile::loadCPUTexture()
{
	m_CPUImageTexture.reset(new CPUTexture(m_GPUImageTexture->download(), m_GPUImageTexture->getWidth(), m_GPUImageTexture->getHeight()));
}

void ImageResourceFile::reimport()
{
	ResourceFile::reimport();

	const FileBuffer& fileBuffer = OS::LoadFileContents(m_Path.generic_string());
	m_GPUImageTexture.reset(new GPUTexture(fileBuffer.data(), fileBuffer.size()));
	if (m_IsCPUAccess)
	{
		loadCPUTexture();
	}
}

void ImageResourceFile::setCPUAccess(bool cpuAccess)
{
	if (!m_IsCPUAccess && cpuAccess)
	{
		loadCPUTexture();
	}
	if (!cpuAccess)
	{
		m_CPUImageTexture.reset();
	}
	m_IsCPUAccess = cpuAccess;
};

void ImageResourceFile::uploadCPUTexturetoGPU()
{
	if (m_IsCPUAccess)
	{
		m_GPUImageTexture.reset(new GPUTexture((const char*)m_CPUImageTexture->getBuffer(), m_CPUImageTexture->getWidth(), m_CPUImageTexture->getHeight()));
	}
}
