#include "image_resource_file.h"

ImageResourceFile::ImageResourceFile(const FilePath& path)
    : ResourceFile(Type::Image, path)
{
	reimport();
}

void ImageResourceFile::reimport()
{
	ResourceFile::reimport();

	const FileBuffer& fileBuffer = OS::LoadFileContents(m_Path.generic_string());
	m_ImageTexture.reset(new Texture(fileBuffer.data(), fileBuffer.size()));
}
