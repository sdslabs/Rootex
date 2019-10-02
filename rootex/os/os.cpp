#include "os.h"

#include <filesystem>
#include <sstream>
#include "common.h"

OS OS::getSingleton()
{
	static OS singleton;
	return singleton;
}

std::string OS::loadFileContents(std::string path)
{
	if (!exists(path))
	{
		ERR("File does not exist: " + path);
		return "";
	}

	std::ifstream stream;
	try
	{
		stream.open(path, std::ios::in);
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

bool OS::exists(std::string path)
{
	return std::filesystem::exists(path);
}
