#include "image_resource_file.h"

#include "resource_loader.h"
#include "framework/systems/render_system.h"

ImageResourceFile::ImageResourceFile(const FilePath& path)
    : ResourceFile(Type::Image, path)
    , m_IsSprite(false)
{
	reimport();
}

void ImageResourceFile::reimport()
{
	ResourceFile::reimport();

	const FileBuffer& fileBuffer = OS::LoadFileContents(m_Path.generic_string());
	m_ImageTexture.reset(new Texture(fileBuffer.data(), fileBuffer.size()));
}

void ImageResourceFile::treatAsSprite()
{
	if (m_IsSprite)
		return;

	FilePath spriteMaterialResourceFilePath = OS::ReplaceExtension(m_Path.generic_string(), String("meta.rmat"));
	m_BasicMaterialResourceFile = ResourceLoader::CreateNewBasicMaterialResourceFile(spriteMaterialResourceFilePath.generic_string());

	m_IsSprite = true;
}

void ImageResourceFile::bindAll()
{
	treatAsSprite();

}
