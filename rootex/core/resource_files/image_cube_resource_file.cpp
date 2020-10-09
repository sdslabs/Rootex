#include "image_cube_resource_file.h"

ImageCubeResourceFile::ImageCubeResourceFile(const FilePath& path)
    : ResourceFile(Type::ImageCube, path)
{
	reimport();
}

void ImageCubeResourceFile::RegisterAPI(sol::table& rootex)
{
	sol::usertype<ImageCubeResourceFile> imageCubeResourceFile = rootex.new_usertype<ImageCubeResourceFile>(
	    "ImageCubeResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
}

void ImageCubeResourceFile::reimport()
{
	ResourceFile::reimport();

	const FileBuffer& fileBuffer = OS::LoadFileContents(m_Path.generic_string());
	m_ImageTexture.reset(new TextureCube(fileBuffer.data(), fileBuffer.size()));
}
