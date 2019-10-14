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

	m_GameDirectory = path / "game";
	m_AssetsDirectory = path / "game" / "assets";
	m_EngineDirectory = path / "rootex";
}

OS::~OS()
{
}

OS OS::getSingleton()
{
	static OS singleton;
	return singleton;
}

std::string OS::getBuildDate()
{
	return std::string(__DATE__);
}

std::string OS::getBuildTime()
{
	return std::string(__TIME__);
}

std::string OS::loadFileContents(DirectoryShortcut directory, std::string stringPath)
{
	std::filesystem::path path = getAbsolutePath(directory, stringPath);

	if (!exists(path.generic_string()))
	{
		ERR("File does not exist: " + path.generic_string());
		return "";
	}

	std::ifstream stream;
	try
	{
		stream.open(path.generic_string(), std::ios::in);
	}
	catch (std::exception e)
	{
		ERR("File IO error: " + e.what());
		return "";
	}

	std::stringstream fileContents;
	fileContents << stream.rdbuf();

	stream.close();
	return fileContents.str();
}

std::filesystem::path OS::getAbsolutePath(DirectoryShortcut directory, std::string stringPath)
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

bool OS::exists(std::string filePath)
{
	return std::filesystem::exists(filePath);
}
