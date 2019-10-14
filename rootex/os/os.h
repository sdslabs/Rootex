#pragma once

#include <fstream>
#include <filesystem>

#define BUILD_SUBDIRECTORY "build"

enum class DirectoryShortcut
{
	ASSETS,
	GAME,
	ENGINE
};

class OS
{
	std::filesystem::path m_GameDirectory;
	std::filesystem::path m_AssetsDirectory;
	std::filesystem::path m_EngineDirectory;

	OS();

public:
	~OS();

	static OS getSingleton();

	std::string getBuildDate();
	std::string getBuildTime();

	std::string loadFileContents(DirectoryShortcut directory, std::string stringPath);
	std::filesystem::path getAbsolutePath(DirectoryShortcut directory, std::string stringPath);
	bool exists(std::string filePath);
};
