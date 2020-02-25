#pragma once

#include <filesystem>
#include <fstream>

#include "common/types.h"

#define ROOT_MARKER_FILENAME "rootex.root"
#define ENGINE_DIRECTORY "rootex"
#define GAME_DIRECTORY "game"

typedef Vector<char> FileBuffer;

class ResourceData;

class OS
{
	OS() = delete;

public:
	static FilePath s_RootDirectory;
	static FilePath s_GameDirectory;
	static FilePath s_EngineDirectory;

	~OS() = delete;

	static bool Initialize();
	static String GetBuildDate();
	static String GetBuildTime();

	static FileBuffer LoadFileContents(String stringPath);
	static bool Exists(String relativePath);
	static FilePath OS::GetAbsolutePath(String stringPath);

	static void Print(const String& msg);
	static void Print(const float& real);
	static void Print(const int& number);
	static void Print(const unsigned int& number);
	static void PrintLine(const String& msg);
	static void PrintWarning(const String& warning);
	static void PrintError(const String& error);
	static void PrintIf(const bool& expr, const String& error);

	static void PostError(String message, LPSTR caption);

	static bool SaveFile(const FilePath& filePath, ResourceData* fileData);
};
