#include "font_resource_file.h"

#include "renderer/rendering_device.h"

FontResourceFile::FontResourceFile(const FilePath& path)
    : ResourceFile(Type::Font, path)
{
	reimport();
}

void FontResourceFile::RegisterAPI(sol::table& rootex)
{
	sol::usertype<FontResourceFile> fontResourceFile = rootex.new_usertype<FontResourceFile>(
	    "FontResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
}

void FontResourceFile::reimport()
{
	ResourceFile::reimport();

	m_Font = RenderingDevice::GetSingleton()->createFont(m_Path.generic_string());
	m_Font->SetDefaultCharacter('_');
}
