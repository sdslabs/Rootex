#pragma once

#include <filesystem>
#include <fstream>

#include "common/types.h"

#define ROOT_MARKER_FILENAME "rootex.root"
#define ENGINE_DIRECTORY "rootex"
#define GAME_DIRECTORY "game"

typedef std::vector<char> FileBuffer;

class OS
{
	OS() = delete;

public:
	static FilePath s_RootDirectory;
	static FilePath s_GameDirectory;
	static FilePath s_EngineDirectory;

	~OS() = delete;

	static bool initialize();
	static String getBuildDate();
	static String getBuildTime();

	static FileBuffer loadFileContents(String stringPath);
	static bool exists(String relativePath);
	static FilePath OS::getAbsolutePath(String stringPath);

	static void print(const String& msg);
	static void printLine(const String& msg);
	static void printWarning(const String& warning);
	static void printError(const String& error);
	static void printIf(const bool& expr, const String& error);
};
