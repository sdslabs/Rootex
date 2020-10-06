#pragma once

#include <filesystem>
#include <fstream>
#include <chrono>

#include "common/types.h"

/// Filename that marks the starting directory for Rootex
#define ROOT_MARKER_FILENAME "rootex.root"
#define ENGINE_DIRECTORY "rootex"
#define GAME_DIRECTORY "game"

typedef Vector<char> FileBuffer;
typedef std::chrono::time_point<std::filesystem::file_time_type::clock> FileTimePoint; 

class ResourceData;

/// Provides features that are provided directly by the OS.
class OS
{
	OS() = delete;

public:
	static std::filesystem::file_time_type::clock s_FileSystemClock;
	static const std::chrono::time_point<std::chrono::system_clock> s_ApplicationStartTime;
	static FilePath s_RootDirectory;
	static FilePath s_GameDirectory;
	static FilePath s_EngineDirectory;

	~OS() = delete;

	static bool Initialize();
	/// Execute a command.
	static void Execute(const String string);

	static bool ElevateThreadPriority();
	static int GetCurrentThreadPriority();

	static String GetBuildDate();
	static String GetBuildTime();
	static String GetBuildType();
	static String GetGameExecutablePath();

	static int GetDisplayWidth();
	static int GetDisplayHeight();

	static void OpenFileInSystemEditor(const String& filePath);
	static void OpenFileInExplorer(const String& filePath);
	static void EditFileInSystemEditor(const String& filePath);
	static FileTimePoint GetFileLastChangedTime(const String& filePath);

	static bool IsExists(String relativePath);
	static FileBuffer LoadFileContents(String stringPath);
	static FilePath GetAbsolutePath(String stringPath);
	static FilePath GetRootRelativePath(String stringPath);
	static FilePath GetRelativePath(String stringPath, String base);

	static Vector<FilePath> GetAllFilesInDirectory(const String& directory);
	static Vector<FilePath> GetAllInDirectory(const String& directory);
	static Vector<FilePath> GetDirectoriesInDirectory(const String& directory);
	static bool DeleteDirectory(const String& dirPath);
	static bool Rename(const String& sourcePath, const String& destinationPath);
	static Vector<FilePath> GetFilesInDirectory(const String& directory);

	static bool IsDirectory(const String& path);
	static bool IsFile(const String& path);

	static void CreateDirectoryName(const String& dirPath);
	static InputOutputFileStream CreateFileName(const String& filePath);

	static bool SaveFile(const FilePath& filePath, ResourceData* fileData);

	static void Print(const String& msg, const String& type = "Print");
	static void Print(const float& real);
	static void Print(const int& number);
	static void Print(const unsigned int& number);
	static void PrintLine(const String& msg);
	static void PrintWarning(const String& warning);
	static void PrintError(const String& error);
	static void PrintIf(const bool& expr, const String& error);

	static void PostError(String message, LPSTR caption);
};
