#include "text_resource_file.h"

TextResourceFile::TextResourceFile(const FilePath& path)
    : ResourceFile(Type::Text, path)
{
	reimport();
}

void TextResourceFile::reimport()
{
	ResourceFile::reimport();
	const FileBuffer& fileBuffer = OS::LoadFileContents(m_Path.generic_string());
	m_FileString = String(fileBuffer.begin(), fileBuffer.end());
}

bool TextResourceFile::save()
{
	bool saved = OS::SaveFile(getPath(), getString().c_str(), getString().size());
	PANIC(saved == false, "Text Resource could not be saved: " + getPath().generic_string());
	return saved;
}

void TextResourceFile::putString(const String& newData)
{
	m_FileString = newData;
}

void TextResourceFile::popBack()
{
	m_FileString.pop_back();
}

void TextResourceFile::append(const String& add)
{
	m_FileString.append(add);
}

String TextResourceFile::getString() const
{
	return m_FileString;
}
