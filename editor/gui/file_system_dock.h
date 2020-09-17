#pragma once

class FileSystemDock
{
public:
	struct FileSystemSettings
	{
		bool m_IsActive = true;
	};

private:
	FileSystemSettings m_FileSystemSettings;
	String m_OpenedFileName;

	void drawFileSystemTree(const FilePath& path);

public:
	FileSystemDock() = default;
	FileSystemDock(FileSystemDock&) = delete;
	~FileSystemDock() = default;

	void draw(float deltaMilliseconds);

	FileSystemSettings& getSettings() { return m_FileSystemSettings; }
	void setActive(bool enabled) { m_FileSystemSettings.m_IsActive = enabled; }
};
