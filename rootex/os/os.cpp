#include "os.h"

#include <filesystem>

#include "common/common.h"

OS::OS()
{
	std::filesystem::path path = std::filesystem::current_path();
	while (true)
	{
		if (std::filesystem::exists(path / ROOT_MARKER_FILENAME))
		{
			break;
		}
		path = path.parent_path();
	}

	m_GameDirectory = path / GAME_DIRECTORY;
	m_AssetsDirectory = path / GAME_DIRECTORY / ASSETS_DIRECTORY;
	m_EngineDirectory = path / ENGINE_DIRECTORY;
}

OS::~OS()
{
}

OS OS::getSingleton()
{
	static OS singleton;
	return singleton;
}

String OS::getBuildDate()
{
	return String(__DATE__);
}

String OS::getBuildTime()
{
	return String(__TIME__);
}

FileBuffer OS::loadFileContents(DirectoryShortcut directory, String stringPath)
{
	std::filesystem::path path = getAbsolutePath(directory, stringPath);

	if (!exists(path.generic_string()))
	{
		ERR("OS: File IO error: " + path.generic_string() + " does not exist");
		return FileBuffer();
	}

	std::ifstream stream;
	try
	{
		stream.open(path.generic_string(), std::ifstream::ate | std::ios::binary);
	}
	catch (std::exception e)
	{
		ERR("OS: File IO error: " + e.what());
		return FileBuffer();
	}

	std::ifstream::pos_type pos = stream.tellg();
	std::vector<char> buffer(pos);

	stream.seekg(0, std::ios_base::beg);
	stream.read(buffer.data(), pos);

	stream.close();
	return FileBuffer(buffer);
}

std::filesystem::path OS::getAbsolutePath(DirectoryShortcut directory, String stringPath)
{
	std::filesystem::path newPath;

	switch (directory)
	{
	case DirectoryShortcut::ASSETS:
		newPath = m_AssetsDirectory / std::filesystem::path(stringPath);
		break;
	case DirectoryShortcut::GAME:
		newPath = m_GameDirectory / std::filesystem::path(stringPath);
		break;
	case DirectoryShortcut::ENGINE:
		newPath = m_EngineDirectory / std::filesystem::path(stringPath);
		break;
	default:
		ERR("ResourceLoader: Directory type not found for converting path to absolute");
		break;
	}

	return newPath;
}

bool OS::exists(String filePath)
{
	return std::filesystem::exists(filePath);
}

FileBuffer::FileBuffer()
    : m_Buffer()
{
}

FileBuffer::FileBuffer(std::vector<char> buffer)
    : m_Buffer(buffer)
{
}
