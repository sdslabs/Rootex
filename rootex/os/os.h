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
	static std::filesystem::path s_GameDirectory;
	static std::filesystem::path s_AssetsDirectory;
	static std::filesystem::path s_EngineDirectory;

	OS() = delete;

public:
	~OS() = delete;

	static bool initialize();
	static String getBuildDate();
	static String getBuildTime();

	static FileBuffer loadFileContents(DirectoryShortcut directory, String stringPath);
	static std::filesystem::path getAbsolutePath(DirectoryShortcut directory, String stringPath);
	static bool exists(String filePath);

	static void print(const String& msg);
	static void printLine(const String& msg);
	static void printWarning(const String& warning);
	static void printError(const String& error);
	static void printIf(const bool& expr, const String& error);
};
