#include "os.h"

#include <filesystem>
#include <sstream>

#include "common/common.h"

OS::OS()
{
	std::filesystem::path path = std::filesystem::current_path();
	while (path.stem() != ENGINE_DIRECTORY)
	{
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
		ERR("File IO error: " + path.generic_string() + " does not exist");
		return FileBuffer(nullptr, 0);
	}

	std::ifstream stream;
	try
	{
		stream.open(path.generic_string(), std::ios::binary | std::ios::in);
	}
	catch (std::exception e)
	{
		ERR("File IO error: " + e.what());
		return FileBuffer(nullptr, 0);
	}

	std::streamsize size = stream.tellg();
	char* buffer = new char(size);
	stream.read(buffer, size);

	stream.close();
	return FileBuffer(buffer, size);
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
		ERR("Directory type not found for converting path to absolute");
		break;
	}

	return newPath;
}

bool OS::exists(String filePath)
{
	return std::filesystem::exists(filePath);
}

FileBuffer::FileBuffer(char* buffer, unsigned int size)
    : m_Buffer(buffer)
    , m_Size(size)
{
}
