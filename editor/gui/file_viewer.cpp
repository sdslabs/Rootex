#include "file_viewer.h"

#include "resource_loader.h"

#include "imgui.h"
#include "Tracy/Tracy.hpp"

void FileViewer::drawFileInfo()
{
	ImGui::Columns(2);

	ImGui::Text("Path");
	ImGui::NextColumn();
	ImGui::Text(OS::GetAbsolutePath(m_OpenFile->getPath().string()).string().c_str());
	ImGui::NextColumn();

	ImGui::Text("Type");
	ImGui::NextColumn();
	ImGui::Text(ResourceFile::s_TypeNames.at(m_OpenFile->getType()).c_str());
	ImGui::NextColumn();

	ImGui::Text("Last Modified");
	ImGui::NextColumn();
	SYSTEMTIME systemTime;
	if (FileTimeToSystemTime((const FILETIME*)&m_OpenFile->getLastChangedTime(), &systemTime))
	{
		ImGui::Text("%d:%d:%d - %d/%d/%d", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wDay, systemTime.wMonth, systemTime.wYear);
	}
	ImGui::NextColumn();

	ImGui::Columns(1);
}

Variant FileViewer::unloadAllResources(const Event* event)
{
	m_AudioPlayer.unload();
	m_ImageViewer.unload();
	m_TextViewer.unload();
	m_MaterialViewer.unload();
	return true;
}

Variant FileViewer::openFile(const Event* event)
{
	m_IsFileOpened = true;
	m_IsEventJustReceived = true;

	unloadAllResources(nullptr);

	VariantVector pathType = Extract<VariantVector>(event->getData());

	String path = Extract<String>(pathType[0]);
	ResourceFile::Type type = (ResourceFile::Type)Extract<int>(pathType[1]);

	const String& ext = FilePath(path).extension().string();
	if (IsFileSupported(ext, ResourceFile::Type::Audio))
	{
		m_OpenFile = m_AudioPlayer.load(path);
	}
	else if (IsFileSupported(ext, ResourceFile::Type::Image))
	{
		m_OpenFile = m_ImageViewer.load(path);
	}
	else if (ext == ".rmat")
	{
		m_OpenFile = m_MaterialViewer.load(path);
	}
	else
	{
		m_OpenFile = m_TextViewer.load(path);
	}

	return true;
}

FileViewer::FileViewer()
    : m_IsFileOpened(false)
    , m_IsEventJustReceived(false)
    , m_Binder(this)
{
	m_Binder.bind(EditorEvents::EditorOpenFile, &FileViewer::openFile);
	m_Binder.bind(RootexEvents::ApplicationExit, &FileViewer::unloadAllResources);
}

void FileViewer::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_IsFileOpened)
	{
		if (m_IsEventJustReceived)
		{
			ImGui::SetNextWindowFocus();
			m_IsEventJustReceived = false;
		}
		if (ImGui::Begin("File Viewer"))
		{
			if (ImGui::Button("Open"))
			{
				OS::OpenFileInSystemEditor(m_OpenFile->getPath().string());
			}
			ImGui::SameLine();
			if (ImGui::Button("Show In File Explorer"))
			{
				OS::OpenFileInExplorer(m_OpenFile->getPath().string());
			}

			drawFileInfo();

			if (IsFileSupported(m_OpenFile->getPath().extension().generic_string(), ResourceFile::Type::Audio))
			{
				m_AudioPlayer.draw(deltaMilliseconds);
			}
			else if (IsFileSupported(m_OpenFile->getPath().extension().generic_string(), ResourceFile::Type::Image))
			{
				m_ImageViewer.draw(deltaMilliseconds);
			}
			else if (m_OpenFile->getPath().extension() == ".rmat")
			{
				m_MaterialViewer.draw(deltaMilliseconds);
			}
			else
			{
				m_TextViewer.draw(deltaMilliseconds);
			}
		}
		ImGui::End();
	}
}
