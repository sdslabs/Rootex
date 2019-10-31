#pragma once

#include <filesystem>
#include <fstream>

#include "common/types.h"

#define ROOT_MARKER_FILENAME "rootex.root"
#define ENGINE_DIRECTORY "rootex"
#define GAME_DIRECTORY "game"
#define ASSETS_DIRECTORY "assets"

struct FileBuffer
{
	std::vector<char> m_Buffer;
	
	FileBuffer();
	FileBuffer(std::vector<char> buffer);
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
