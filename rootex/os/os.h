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

std::wstring StringToWideString(const String& str);
String WideStringToString(const std::wstring& wstr);

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
	static void Execute(const String& string);
	static void RunApplication(const String& commandLine);

	static bool ElevateThreadPriority();
	static int GetCurrentThreadPriority();

	static String GetBuildDate();
	static String GetBuildTime();
	static String GetBuildType();
	static String GetGameExecutablePath();
	static String GetOrganizationName();

	static String GetAppDataFolder();
	static String GetAbsoluteSaveGameFolder(const String& appName);

	static int GetDisplayWidth();
	static int GetDisplayHeight();

	/// Open a dialog box which the user can select a file from. Selected filepath is returned if successful.
	/// Filter needs 2 '\0' characters at the end.
	static Optional<String> SelectFile(const char* filter, const char* dir = nullptr);
	static Optional<String> SaveSelectFile(const char* filter, const char* dir = nullptr);

	static void OpenFileInSystemEditor(const String& filePath);
	static void OpenFileInExplorer(const String& filePath);
	static void EditFileInSystemEditor(const String& filePath);
	static FileTimePoint GetFileLastChangedTime(const String& filePath);

	static bool IsExistsAbsolute(String absPath);
	static bool IsExists(String relativePath);
	static FileBuffer LoadFileContents(String stringPath);
	static JSON::json LoadFileContentsToJSONObject(String stringPath);
	static FileBuffer LoadFileContentsAbsolute(String absPath);
	static FilePath GetAbsolutePath(String stringPath);
	static FilePath GetRootRelativePath(String stringPath);
	static FilePath GetRelativePath(String stringPath, String base);

	static Vector<FilePath> GetAllFilesInDirectory(const String& directory);
	static Vector<FilePath> GetAllInDirectory(const String& directory);
	static Vector<FilePath> GetDirectoriesInDirectory(const String& directory);
	static bool DeleteDirectory(const String& dirPath);
	static bool Rename(const String& sourcePath, const String& destinationPath);
	static Vector<FilePath> GetFilesInDirectory(const String& directory);
	static bool RelativeCopyFile(const String& src, const String& dest);
	static void RelativeCopyDirectory(const String& src, const String& dest);

	static bool IsDirectory(const String& path);
	static bool IsFile(const String& path);

	static bool CreateDirectoryName(const String& dirPath);
	static bool CreateDirectoryAbsoluteName(const String& dirPath);
	static InputOutputFileStream CreateFileName(const String& filePath);
	static InputOutputFileStream CreateFileNameAbsolute(const String& absFilePath);

	static bool SaveFile(const FilePath& filePath, const char* fileBuffer, size_t fileSize);
	static bool SaveFileAbsolute(const FilePath& absFilePath, const char* fileBuffer, size_t fileSize);

	static void Print(const String& msg, const String& type = "Print");
	static void PrintInline(const String& msg, const String& type = "Print");
	static void Print(const float& real);
	static void Print(const int& number);
	static void Print(const unsigned int& number);
	static void PrintLine(const String& msg);
	static void PrintWarning(const String& warning);
	static void PrintWarningInline(const String& warning);
	static void PrintError(const String& error);
	static void PrintErrorInline(const String& error);
	static void PrintIf(const bool& expr, const String& error);

	static void PrintSilent(const String& msg);
	static void PrintInlineSilent(const String& msg);
	static void PrintSilent(const float& real);
	static void PrintSilent(const int& number);
	static void PrintSilent(const unsigned int& number);
	static void PrintLineSilent(const String& msg);
	static void PrintWarningSilent(const String& warning);
	static void PrintWarningInlineSilent(const String& warning);
	static void PrintErrorSilent(const String& error);
	static void PrintErrorInlineSilent(const String& error);
	static void PrintIfSilent(const bool& expr, const String& error);

	static void PostError(String message, LPSTR caption);
};
