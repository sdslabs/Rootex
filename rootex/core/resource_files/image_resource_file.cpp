#include "image_resource_file.h"

ImageResourceFile::ImageResourceFile(const FilePath& path)
	: ResourceFile(Type::Image, path)
{
	reimport();
}

void ImageResourceFile::RegisterAPI(sol::table& rootex)
{
	sol::usertype<ImageResourceFile> imageResourceFile = rootex.new_usertype<ImageResourceFile>(
		"ImageResourceFile",
		sol::base_classes, sol::bases<ResourceFile>());
}

void ImageResourceFile::reimport()
{
	ResourceFile::reimport();

	const FileBuffer& fileBuffer = OS::LoadFileContents(m_Path.generic_string());
	m_ImageTexture.reset(new Texture(fileBuffer.data(), fileBuffer.size()));
}
