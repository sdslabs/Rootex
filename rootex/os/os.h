#pragma once

#include <filesystem>
#include <fstream>

#include "common/types.h"

#define ENGINE_DIRECTORY "Rootex"
#define GAME_DIRECTORY "game"
#define ASSETS_DIRECTORY "assets"

struct FileBuffer
{
	char* m_Buffer;
	unsigned int m_Size;

	FileBuffer(char* buffer, unsigned int size);
};

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

	String getBuildDate();
	String getBuildTime();

	FileBuffer loadFileContents(DirectoryShortcut directory, String stringPath);
	std::filesystem::path getAbsolutePath(DirectoryShortcut directory, String stringPath);
	bool exists(String filePath);
};
