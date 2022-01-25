#include "os.h"

#include <iostream>
#include <filesystem>
#include <codecvt>

#include "event_manager.h"

#include <commdlg.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj_core.h>
#include <strsafe.h>
#include <windows.h>

std::filesystem::file_time_type::clock OS::s_FileSystemClock;
const std::chrono::time_point<std::chrono::system_clock> OS::s_ApplicationStartTime = std::chrono::system_clock::now();
FilePath OS::s_RootDirectory;
FilePath OS::s_EngineDirectory;
FilePath OS::s_GameDirectory;

std::wstring StringToWideString(const String& str)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.from_bytes(str);
}

String WideStringToString(const std::wstring& wstr)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

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

String OS::GetFileStem(String stringPath)
{
	return GetAbsolutePath(stringPath).filename().generic_string();
}

FilePath OS::GetParentPath(String stringPath)
{
	return GetAbsolutePath(stringPath).parent_path();
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
	try
	{
		std::filesystem::remove_all(GetAbsolutePath(dirPath));

		PRINT("Deleted directory: " + dirPath);
		return true;
	}
	catch (const std::exception& e)
	{
		ERR("Exception while deleting directory: " + dirPath + ": " + e.what());
	}

	return false;
}

bool OS::Rename(const String& sourcePath, const String& destinationPath)
{
	try
	{
		std::filesystem::rename(GetAbsolutePath(sourcePath), GetAbsolutePath(destinationPath));
		PRINT("Renamed " + sourcePath + " to " + destinationPath);
		return true;
	}
	catch (std::exception e)
	{
		PRINT("Could not rename " + sourcePath + " to " + destinationPath + ": " + e.what());
	}
	return false;
}

Vector<FilePath> OS::GetAllFilesInDirectory(const String& directory)
{
	if (!std::filesystem::is_directory(GetAbsolutePath(directory)))
	{
		WARN("Not a directory: " + directory);
		return {};
	}

	Vector<FilePath> result;
	for (auto&& file : std::filesystem::recursive_directory_iterator(GetAbsolutePath(directory)))
	{
		if (!file.is_directory())
		{
			result.push_back(GetRootRelativePath(((FilePath)file).generic_string()));
		}
	}
	return result;
}

Vector<FilePath> OS::GetAllInDirectoryRoot(const String& directory)
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

Vector<FilePath> OS::GetAllInDirectory(const String& directory)
{
	if (!std::filesystem::is_directory(GetAbsolutePath(directory)))
	{
		WARN("Not a directory: " + directory);
		return {};
	}

	Vector<FilePath> result;
	for (auto&& file : std::filesystem::recursive_directory_iterator(GetAbsolutePath(directory)))
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

		if (!SetCurrentDirectory(s_RootDirectory.generic_string().c_str()))
		{
			ERR("SetCurrentDirectory failed (%d)\n");
			Print((unsigned int)GetLastError());
			return false;
		}
	}
	catch (std::exception e)
	{
		ERR("Failed to initialize OS provided functionalities. " + String(e.what()));
		return false;
	}

	return true;
}

void OS::Execute(const String& string)
{
	std::system(string.c_str());
}

void OS::RunApplication(const String& commandLine)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(NULL,
	        (char*)commandLine.c_str(),
	        NULL,
	        NULL,
	        FALSE,
	        0,
	        NULL,
	        NULL,
	        &si,
	        &pi))
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}
}

bool OS::ElevateThreadPriority()
{
	return SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST) != 0;
}

int OS::GetCurrentThreadPriority()
{
	return GetThreadPriority(GetCurrentThread());
}

String OS::GetBuildDate()
{
	return __DATE__;
}

String OS::GetBuildTime()
{
	return __TIME__;
}

String OS::GetBuildType()
{
	return CMAKE_INTDIR;
}

String OS::GetGameExecutablePath()
{
	return GetAbsolutePath("build/game/" + GetBuildType() + "/Game.exe").generic_string();
}

String OS::GetOrganizationName()
{
	return "SDSLabs";
}

String OS::GetAppDataFolder()
{
	String appDataFolderString;

	PWSTR appDataFolder = nullptr;
	if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &appDataFolder)))
	{
		WARN("Could not get or create AppData folder. Using save directory in game directory.");
		CreateDirectoryName("save");
		appDataFolderString = GetAbsolutePath("save").generic_string();
	}
	else
	{
		appDataFolderString = WideStringToString(std::wstring(appDataFolder));
	}
	CoTaskMemFree(appDataFolder);

	return appDataFolderString;
}

String OS::GetAbsoluteSaveGameFolder(const String& appName)
{
	static String appDataFolder = GetAppDataFolder();
	return appDataFolder + "/" + GetOrganizationName() + "/" + appName;
}

int OS::GetDisplayWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

int OS::GetDisplayHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

Optional<String> OS::SelectFile(const char* filter, const char* dir)
{
	OPENFILENAME ofn;
	char szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = dir ? dir : ".";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		return GetRootRelativePath((String)ofn.lpstrFile).generic_string();
	}
	return {};
}

Optional<String> OS::SaveSelectFile(const char* filter, const char* dir)
{
	OPENFILENAME ofn;
	char szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = dir ? dir : ".";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		return GetRootRelativePath((String)ofn.lpstrFile).generic_string();
	}
	return {};
}

bool OS::IsDirectory(const String& path)
{
	return std::filesystem::is_directory(GetAbsolutePath(path));
}

bool OS::IsFile(const String& path)
{
	return std::filesystem::is_regular_file(GetAbsolutePath(path));
}

void OS::RegisterFileSystemWatcher(const String& path, void (*callback)(PVOID, BOOLEAN))
{
	String absolute_path = GetAbsolutePath(path).string();
	LPTSTR lpDir = new TCHAR[absolute_path.size() + 1];
	strcpy(lpDir, absolute_path.c_str());
	std::cout << "Long String is : " << lpDir << std::endl;
	DWORD dwWaitStatus;
	HANDLE dwChangeHandles[2];
	TCHAR lpDrive[4];
	TCHAR lpFile[_MAX_FNAME];
	TCHAR lpExt[_MAX_EXT];

	HANDLE waitHandle;
	int arg = 123;

	_tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

	lpDrive[2] = (TCHAR)'\\';
	lpDrive[3] = (TCHAR)'\0';

	// Watch the directory for file creation and deletion.

	dwChangeHandles[0] = FindFirstChangeNotification(
	    lpDir, // directory to watch
	    FALSE, // do not watch subtree
	    FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes

	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE)
	{
		printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		ExitProcess(GetLastError());
	}

	// Watch the subtree for directory creation and deletion.

	dwChangeHandles[1] = FindFirstChangeNotification(
	    lpDrive, // directory to watch
	    TRUE, // watch the subtree
	    FILE_NOTIFY_CHANGE_DIR_NAME); // watch dir name changes

	if (dwChangeHandles[1] == INVALID_HANDLE_VALUE)
	{
		printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		ExitProcess(GetLastError());
	}

	// Make a final validation check on our handles.

	if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
	{
		printf("\n ERROR: Unexpected NULL from FindFirstChangeNotification.\n");
		ExitProcess(GetLastError());
	}

	
	// Change notification is set. Now wait on both notification
	// handles and refresh accordingly.
	if (!RegisterWaitForSingleObject(&waitHandle, dwChangeHandles[0], (WAITORTIMERCALLBACK)callback, &arg, INFINITE, WT_EXECUTEDEFAULT | WT_EXECUTEONLYONCE))
	{
		WARN("ERROR: Could not register file watcher notifier");
	}
	if (!RegisterWaitForSingleObject(&waitHandle, dwChangeHandles[1], (WAITORTIMERCALLBACK)callback, &arg, INFINITE, WT_EXECUTEDEFAULT | WT_EXECUTEONLYONCE))
	{
		WARN("ERROR: Could not register file watcher notifier");
	}
	/*
	while (TRUE)
	{
		// Wait for notification.

		printf("\nWaiting for notification...\n");

		dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles,
		    FALSE, INFINITE);

		switch (dwWaitStatus)
		{
		case WAIT_OBJECT_0:

			// A file was created, renamed, or deleted in the directory.
			// Refresh this directory and restart the notification.

			//RefreshDirectory(lpDir);
			if (FindNextChangeNotification(dwChangeHandles[0]) == FALSE)
			{
				printf("\n ERROR: FindNextChangeNotification function failed.\n");
				ExitProcess(GetLastError());
			}
			break;

		case WAIT_OBJECT_0 + 1:

			// A directory was created, renamed, or deleted.
			// Refresh the tree and restart the notification.

			//RefreshTree(lpDrive);
			if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE)
			{
				printf("\n ERROR: FindNextChangeNotification function failed.\n");
				ExitProcess(GetLastError());
			}
			break;

		case WAIT_TIMEOUT:

			// A timeout occurred, this would happen if some value other
			// than INFINITE is used in the Wait call and no changes occur.
			// In a single-threaded environment you might not want an
			// INFINITE wait.

			printf("\nNo changes in the timeout period.\n");
			break;

		default:
			printf("\n ERROR: Unhandled dwWaitStatus.\n");
			ExitProcess(GetLastError());
			break;
		}
	}*/
}

bool OS::CreateDirectoryName(const String& dirPath)
{
	FilePath path = OS::GetAbsolutePath(dirPath);
	return CreateDirectoryAbsoluteName(path.generic_string());
}

bool OS::CreateDirectoryAbsoluteName(const String& dirPath)
{
	if (IsExists(dirPath))
	{
		PRINT("Directory already exists: " + dirPath);
		return true;
	}

	if (std::filesystem::create_directories(dirPath))
	{
		PRINT_SILENT("Created directory: " + dirPath);
		return true;
	}
	WARN("Could not create absolute directory: " + dirPath);
	return false;
}

InputOutputFileStream OS::CreateFileName(const String& filePath)
{
	return CreateFileNameAbsolute(OS::GetAbsolutePath(filePath).generic_string());
}

InputOutputFileStream OS::CreateFileNameAbsolute(const String& absFilePath)
{
	InputOutputFileStream file(OS::GetAbsolutePath(absFilePath), std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

	if (file)
	{
		PRINT("Created file: " + absFilePath);
	}
	else
	{
		WARN("Could not create file: " + absFilePath);
	}

	return file;
}

void OS::OpenFileInSystemEditor(const String& filePath)
{
	const String& absolutePath = GetAbsolutePath(filePath).generic_string();
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

	const String& absolutePath = GetAbsolutePath(filePath).parent_path().generic_string();
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
	const String& absolutePath = GetAbsolutePath(filePath).generic_string();
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

bool OS::RelativeCopyFile(const String& src, const String& dest)
{
	String destParent = FilePath(dest).parent_path().generic_string();
	if (!IsDirectory(destParent))
	{
		CreateDirectoryName(destParent);
	}
	bool success = std::filesystem::copy_file(GetAbsolutePath(src), GetAbsolutePath(dest));
	if (!success)
	{
		WARN_SILENT("Copy Failed: " + src + " -> " + dest);
	}
	else
	{
		PRINT_SILENT("Copy Successful: " + src + " -> " + dest);
	}
	return success;
}

void OS::RelativeCopyDirectory(const String& src, const String& dest)
{
	std::filesystem::copy(GetAbsolutePath(src), GetAbsolutePath(dest), std::filesystem::copy_options::recursive);
	PRINT_SILENT("Copy Successful: " + src + " -> " + dest);
}

FileBuffer OS::LoadFileContents(String stringPath)
{
	std::filesystem::path path = GetAbsolutePath(stringPath);
	return LoadFileContentsAbsolute(path.generic_string());
}

JSON::json OS::LoadFileContentsToJSONObject(String stringPath)
{
	const FileBuffer& fileBuffer = LoadFileContents(stringPath);
	String fileString(fileBuffer.begin(), fileBuffer.end());

	JSON::json j = JSON::json::object();
	try
	{
		j = JSON::json::parse(fileString);
	}
	catch (std::exception j)
	{
		WARN("File not loaded correctly into JSON: " + stringPath);
	}
	return j;
}

FileBuffer OS::LoadFileContentsAbsolute(String absPath)
{
	if (!IsExistsAbsolute(absPath))
	{
		ERR("OS: File IO error: " + absPath + " does not exist");
		return FileBuffer();
	}

	std::ifstream stream;
	try
	{
		stream.open(absPath, std::ifstream::ate | std::ios::binary);
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

bool OS::IsExistsAbsolute(String absPath)
{
	return std::filesystem::exists(absPath);
}

bool OS::IsExists(String relativePath)
{
	return std::filesystem::exists(OS::GetAbsolutePath(relativePath));
}

void OS::Print(const String& msg, const String& type)
{
	PrintInline(msg, type);
	std::cout << std::endl;
}

void OS::PrintInline(const String& msg, const String& type)
{
	EventManager::GetSingleton()->call(RootexEvents::OSPrint, VariantVector { msg, type });
	std::cout.clear();
	std::cout << msg;
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

void OS::PrintWarningInline(const String& warning)
{
	std::cout << "\033[93m";
	PrintInline("WARNING: " + warning, "Warning");
	std::cout << "\033[0m";
}

void OS::PrintError(const String& error)
{
	std::cout << "\033[91m";
	Print("ERROR: " + error, "Error");
	std::cout << "\033[0m";
	PostError(error, "Error");
}

void OS::PrintErrorInline(const String& error)
{
	std::cout << "\033[91m";
	PrintInline("ERROR: " + error, "Error");
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

void OS::PrintSilent(const String& msg)
{
	PrintInlineSilent(msg);
	std::cout << std::endl;
}

void OS::PrintInlineSilent(const String& msg)
{
	std::cout.clear();
	std::cout << msg;
}

void OS::PrintSilent(const float& real)
{
	PrintSilent(std::to_string(real));
}

void OS::PrintSilent(const int& number)
{
	PrintSilent(std::to_string(number));
}

void OS::PrintSilent(const unsigned int& number)
{
	PrintSilent(std::to_string(number));
}

void OS::PrintLineSilent(const String& msg)
{
	PrintSilent(msg);
}

void OS::PrintWarningSilent(const String& warning)
{
	std::cout << "\033[93m";
	PrintSilent("WARNING: " + warning);
	std::cout << "\033[0m";
}

void OS::PrintWarningInlineSilent(const String& warning)
{
	std::cout << "\033[93m";
	PrintInlineSilent("WARNING: " + warning);
	std::cout << "\033[0m";
}

void OS::PrintErrorSilent(const String& error)
{
	std::cout << "\033[91m";
	PrintSilent("ERROR: " + error);
	std::cout << "\033[0m";
	PostError(error, "Error");
}

void OS::PrintErrorInlineSilent(const String& error)
{
	std::cout << "\033[91m";
	PrintInlineSilent("ERROR: " + error);
	std::cout << "\033[0m";
	PostError(error, "Error");
}

void OS::PrintIfSilent(const bool& expr, const String& error)
{
	if (expr)
	{
		PrintErrorSilent(error);
	}
}

void OS::PostError(String message, LPSTR caption)
{
	MessageBoxA(GetActiveWindow(), message.c_str(), caption, MB_OK);
}

bool OS::SaveFile(const FilePath& filePath, const char* fileBuffer, size_t fileSize)
{
	return SaveFileAbsolute(GetAbsolutePath(filePath.generic_string()), fileBuffer, fileSize);
}

bool OS::SaveFileAbsolute(const FilePath& absFilePath, const char* fileBuffer, size_t fileSize)
{
	std::ofstream outFile;

	try
	{
		outFile.open(absFilePath.generic_string(), std::ios::out | std::ios::binary);
		outFile.write(fileBuffer, fileSize);
	}
	catch (std::exception e)
	{
		ERR(std::string("File IO error: ") + std::string(e.what()));
		return false;
	}

	outFile.close();
	return true;
}
