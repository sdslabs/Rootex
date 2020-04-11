#include "os.h"

#include <iostream>
#include <filesystem>
#include <iostream>

#include "common/common.h"
#include "resource_data.h"

#ifdef ROOTEX_EDITOR
#include "event_manager.h"
#endif // ROOTEX_EDITOR

std::filesystem::file_time_type::clock OS::s_FileSystemClock;
const std::chrono::time_point<std::chrono::system_clock> OS::s_ApplicationStartTime = std::chrono::system_clock::now();
FilePath OS::s_RootDirectory;
FilePath OS::s_EngineDirectory;
FilePath OS::s_GameDirectory;

FilePath OS::GetAbsolutePath(String stringPath)
{
	FilePath absPath = s_RootDirectory / stringPath;

	return absPath;
}

FilePath OS::GetRootRelativePath(String stringPath)
{
	return std::filesystem::relative(stringPath, s_RootDirectory);
}

FilePath OS::GetRelativePath(String stringPath, String base)
{
	return std::filesystem::relative(GetAbsolutePath(stringPath), GetAbsolutePath(base));
}

Vector<FilePath> OS::GetDirectoriesInDirectory(const String& directory)
{
	if (!std::filesystem::is_directory(GetAbsolutePath(directory)))
	{
		WARN("Not a directory: " + directory);
		return {};
	}

	Vector<FilePath> result;
	for (auto&& file : std::filesystem::directory_iterator(GetAbsolutePath(directory)))
	{
		if (file.is_directory())
		{
			result.push_back(GetRootRelativePath(((FilePath)file).generic_string()));
		}
	}
	return result;
}

bool OS::DeleteDirectory(const String& dirPath)
{
	bool status = std::filesystem::remove_all(GetAbsolutePath(dirPath));

	if (status)
	{
		PRINT("Deleted directory: " + dirPath);
	}

	return status;
}

void OS::Rename(const String& sourcePath, const String& destinationPath)
{
	std::filesystem::rename(GetAbsolutePath(sourcePath), GetAbsolutePath(destinationPath));
	PRINT("Renamed " + sourcePath + " to " + destinationPath);
}

Vector<FilePath> OS::GetAllInDirectory(const String& directory)
{
	if (!std::filesystem::is_directory(GetAbsolutePath(directory)))
	{
		WARN("Not a directory: " + directory);
		return {};
	}

	Vector<FilePath> result;
	for (auto&& file : std::filesystem::directory_iterator(GetAbsolutePath(directory)))
	{
		result.push_back(GetRootRelativePath(((FilePath)file).generic_string()));
	}
	return result;
}

Vector<FilePath> OS::GetFilesInDirectory(const String& directory)
{
	if (!std::filesystem::is_directory(GetAbsolutePath(directory)))
	{
		WARN("Not a directory: " + directory);
		return {};
	}

	Vector<FilePath> result;
	for (auto&& file : std::filesystem::directory_iterator(GetAbsolutePath(directory)))
	{
		if (!file.is_directory())
		{
			result.push_back(GetRootRelativePath(((FilePath)file).generic_string()));
		}
	}
	return result;
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

		if (!SetCurrentDirectory(s_RootDirectory.string().c_str()))
		{
			ERR("SetCurrentDirectory failed (%d)\n");
			Print((unsigned int)GetLastError());
			return false;
		}
	}
	catch (std::exception e)
	{
		ERR("OS: Failed to Initialize OS: " + String(e.what()));
		return false;
	}

	return true;
}

void OS::Execute(const String string)
{
	std::system(string.c_str());
}

String OS::GetBuildDate()
{
	return String(__DATE__);
}

String OS::GetBuildTime()
{
	return String(__TIME__);
}

String OS::GetBuildType()
{
#ifdef _DEBUG
	return "Debug";
#else
	return "Release";
#endif // _DEBUG
}

String OS::GetGameExecutablePath()
{
	return GetAbsolutePath("build/game/" + GetBuildType() + "/Game.exe").string();
}

bool OS::IsDirectory(const String& path)
{
	return std::filesystem::is_directory(GetAbsolutePath(path));
}

bool OS::IsFile(const String& path)
{
	return std::filesystem::is_regular_file(GetAbsolutePath(path));
}

void OS::CreateDirectoryName(const String& dirPath)
{
	FilePath path = OS::GetAbsolutePath(dirPath);

	if (IsExists(dirPath))
	{
		WARN("Directory already exists: " + dirPath);
		return;
	}

	std::filesystem::create_directories(path);
	PRINT("Created directory: " + path.string());
}

InputOutputFileStream OS::CreateFileName(const String& filePath)
{
	FilePath path = OS::GetAbsolutePath(filePath);

	InputOutputFileStream file(OS::GetAbsolutePath(filePath), std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

	if (file)
	{
		if (!IsExists(filePath))
		{
			PRINT("Created file: " + path.string());
		}
	}
	else
	{
		WARN("Could not create directory: " + path.string());
	}

	return file;
}

void OS::OpenFileInSystemEditor(const String& filePath)
{
	const String& absolutePath = GetAbsolutePath(filePath).string();
	HINSTANCE error = 0;
	SHELLEXECUTEINFO cmdInfo;
	ZeroMemory(&cmdInfo, sizeof(cmdInfo));
	cmdInfo.cbSize = sizeof(cmdInfo);
	cmdInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	cmdInfo.hwnd = GetActiveWindow();
	cmdInfo.lpVerb = "open";
	cmdInfo.lpFile = absolutePath.c_str();
	cmdInfo.nShow = SW_SHOW;
	cmdInfo.hInstApp = error;

	ShellExecuteEx(&cmdInfo);

	switch ((int)error)
	{
	case SE_ERR_FNF:
		ERR("File not found: " + filePath);
		break;
	case SE_ERR_ASSOCINCOMPLETE:
	case SE_ERR_NOASSOC:
		ERR("File association not found on system: " + filePath);
		break;
	}
}

void OS::OpenFileInExplorer(const String& filePath)
{
	if (!IsExists(filePath))
	{
		WARN("File not found: " + filePath);
		return;
	}

	const String& absolutePath = GetAbsolutePath(filePath).parent_path().string();
	HINSTANCE error = 0;
	SHELLEXECUTEINFO cmdInfo;
	ZeroMemory(&cmdInfo, sizeof(cmdInfo));
	cmdInfo.cbSize = sizeof(cmdInfo);
	cmdInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	cmdInfo.hwnd = GetActiveWindow();
	cmdInfo.lpVerb = "explore";
	cmdInfo.lpFile = absolutePath.c_str();
	cmdInfo.nShow = SW_SHOW;
	cmdInfo.hInstApp = error;

	ShellExecuteEx(&cmdInfo);

	switch ((int)error)
	{
	case SE_ERR_FNF:
		ERR("File not found: " + filePath);
		break;
	case SE_ERR_ASSOCINCOMPLETE:
	case SE_ERR_NOASSOC:
		ERR("File association not found on system: " + filePath);
		break;
	}
}

void OS::EditFileInSystemEditor(const String& filePath)
{
	const String& absolutePath = GetAbsolutePath(filePath).string();
	HINSTANCE error = 0;
	SHELLEXECUTEINFO cmdInfo;
	ZeroMemory(&cmdInfo, sizeof(cmdInfo));
	cmdInfo.cbSize = sizeof(cmdInfo);
	cmdInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	cmdInfo.hwnd = GetActiveWindow();
	cmdInfo.lpVerb = "edit";
	cmdInfo.lpFile = absolutePath.c_str();
	cmdInfo.nShow = SW_SHOW;
	cmdInfo.hInstApp = error;

	ShellExecuteEx(&cmdInfo);

	switch ((int)error)
	{
	case SE_ERR_FNF:
		ERR("File not found: " + filePath);
		break;
	case SE_ERR_ASSOCINCOMPLETE:
	case SE_ERR_NOASSOC:
		ERR("File association not found on system: " + filePath);
		break;
	}
}

FileTimePoint OS::GetFileLastChangedTime(const String& filePath)
{
	FileTimePoint result = FileTimePoint::clock::now();

	try
	{
		result = std::filesystem::last_write_time(GetAbsolutePath(filePath));
	}
	catch (std::exception e)
	{
		ERR("Cannot find last changed time for " + filePath + ": " + String(e.what()));
	}

	return result;
}

FileBuffer OS::LoadFileContents(String stringPath)
{
	std::filesystem::path path = GetAbsolutePath(stringPath);

	if (!IsExists(path.generic_string()))
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

bool OS::IsExists(String relativePath)
{
	return std::filesystem::exists(OS::GetAbsolutePath(relativePath));
}

void OS::Print(const String& msg, const String& type)
{
#ifdef ROOTEX_EDITOR
	EventManager::GetSingleton()->call(type, "OSPrint", msg);
#endif // ROOTEX_EDITOR
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
	Print("WARNING: " + warning, "Warning");
	std::cout << "\033[0m";
}

void OS::PrintError(const String& error)
{
	std::cout << "\033[91m";
	Print("ERROR: " + error, "Error");
	std::cout << "\033[0m";
	PostError(error, "Error");
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
