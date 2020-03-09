#include "os.h"

#include <iostream>
#include <filesystem>

#include "common/common.h"
#include "resource_data.h"

FilePath OS::s_RootDirectory;
FilePath OS::s_EngineDirectory;
FilePath OS::s_GameDirectory;

FilePath OS::GetAbsolutePath(String stringPath)
{
	FilePath absPath = s_RootDirectory / stringPath;

	return absPath;
}

bool OS::Initialize()
{
	try
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

		s_RootDirectory = path;
		s_GameDirectory = path / GAME_DIRECTORY;
		s_EngineDirectory = path / ENGINE_DIRECTORY;
	}
	catch (std::exception e)
	{
		ERR("OS: Failed to Initialize OS: " + String(e.what()));
		return false;
	}

	return true;
}

String OS::GetBuildDate()
{
	return String(__DATE__);
}

String OS::GetBuildTime()
{
	return String(__TIME__);
}

FileBuffer OS::LoadFileContents(String stringPath)
{
	std::filesystem::path path = GetAbsolutePath(stringPath);

	if (!Exists(path.generic_string()))
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
		ERR(std::string("OS: File IO error: ") + std::string(e.what()));
		return FileBuffer();
	}

	std::ifstream::pos_type pos = stream.tellg();
	std::vector<char> buffer(pos);

	stream.seekg(0, std::ios_base::beg);
	stream.read(buffer.data(), pos);

	stream.close();
	return FileBuffer(buffer);
}

bool OS::Exists(String relativePath)
{
	return std::filesystem::exists(OS::GetAbsolutePath(relativePath));
}

void OS::Print(const String& msg)
{
	std::cout.clear();
	std::cout << msg << std::endl;
}

void OS::Print(const float& real)
{
	Print(std::to_string(real));
}

void OS::Print(const int& number)
{
	Print(std::to_string(number));
}

void OS::Print(const unsigned int& number)
{
	Print(std::to_string(number));
}

void OS::PrintLine(const String& msg)
{
	Print(msg);
}

void OS::PrintWarning(const String& warning)
{
	std::cout << "\033[93m";
	Print(warning);
	std::cout << "\033[0m";
}

void OS::PrintError(const String& error)
{
	std::cout.clear();
	std::cout << "\033[91m";
	Print(error);
	std::cout << "\033[0m";
	PostError(error, "Fatal Error");
}

void OS::PrintIf(const bool& expr, const String& error)
{
	if (expr)
	{
		PrintError(error);
	}
}

void OS::PostError(String message, LPSTR caption)
{
	MessageBoxA(GetActiveWindow(), message.c_str(), caption, MB_OK);
}

bool OS::SaveFile(const FilePath& filePath, ResourceData* fileData)
{
	std::ofstream outFile;

	try
	{
		outFile.open(GetAbsolutePath(filePath.generic_string()), std::ios::out | std::ios::binary);
		outFile.write(fileData->getRawData()->data(), fileData->getRawDataByteSize());
	}
	catch (std::exception e)
	{
		ERR(std::string("OS: File IO error: ") + std::string(e.what()));
		return false;
	}
	
	outFile.close();
	return true;
}
