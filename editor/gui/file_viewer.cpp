#include "file_viewer.h"

#include "resource_loader.h"

#include "imgui.h"

void FileViewer::drawFileInfo()
{
	ImGui::Columns(2);

	ImGui::Text("Path");
	ImGui::NextColumn();
	ImGui::Text(OS::GetAbsolutePath(m_OpenFile->getPath().string()).string().c_str());
	ImGui::NextColumn();

	ImGui::Text("Type");
	ImGui::NextColumn();
	ImGui::Text(m_OpenFilePath.extension().string().c_str());
	ImGui::NextColumn();

	ImGui::Text("Size");
	ImGui::NextColumn();
	float size = m_OpenFile->getData()->getRawDataByteSize();
	float sizeUI = size;
	String sizeUnitUI = " B";
	if (sizeUI > 1.0f / B_TO_KB)
	{
		sizeUI = size * B_TO_KB;
		sizeUnitUI = " KB";
		if (sizeUI > 1.0f / KB_TO_MB)
		{
			sizeUI = size * B_TO_KB * KB_TO_MB;
			sizeUnitUI = " MB";
		}
	}
	ImGui::Text((std::to_string(sizeUI) + sizeUnitUI).c_str());
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

Variant FileViewer::openFile(const Event* event)
{
	m_IsFileOpened = true;
	m_IsEventJustReceived = true;

	m_AudioPlayer.unload();
	m_ImageViewer.unload();
	m_TextViewer.unload();

	m_OpenFilePath = Extract(String, event->getData());

	const String& ext = m_OpenFilePath.extension().string();
	if (ext == ".wav")
	{
		m_OpenFile = m_AudioPlayer.load(m_OpenFilePath);
	}
	else if (ext == ".jpg" || ext == ".png" || ext == ".jpeg")
	{
		m_OpenFile = m_ImageViewer.load(m_OpenFilePath);
	}
	else
	{
		m_OpenFile = m_TextViewer.load(m_OpenFilePath);
	}

	return true;
}

FileViewer::FileViewer()
    : m_IsFileOpened(false)
    , m_IsEventJustReceived(false)
{
	BIND_EVENT_MEMBER_FUNCTION("EditorOpenFile", openFile);
}

FileViewer::~FileViewer()
{
}

void FileViewer::draw(float deltaMilliseconds)
{
	if (m_IsFileOpened)
	{
		if (m_IsEventJustReceived)
		{
			ImGui::SetNextWindowFocus();
			m_IsEventJustReceived = false;
		}
		if (ImGui::Begin("File Viewer"))
		{
			ImGui::Separator();
			if (ImGui::Button("Open"))
			{
				OS::OpenFileInSystemEditor(m_OpenFilePath.string());
			}
			ImGui::SameLine();
			if (ImGui::Button("Edit"))
			{
				OS::EditFileInSystemEditor(m_OpenFilePath.string());
			}
			ImGui::SameLine();
			if (ImGui::Button("Open In File Explorer"))
			{
				OS::OpenFileInExplorer(m_OpenFilePath.string());
			}

			drawFileInfo();
			
			if (m_OpenFilePath.extension() == ".wav")
			{
				m_AudioPlayer.draw(deltaMilliseconds);
			}
			else if (m_OpenFilePath.extension() == ".png" || m_OpenFilePath.extension() == ".jpg" || m_OpenFilePath.extension() == ".jpeg")
			{
				m_ImageViewer.draw(deltaMilliseconds);
			}
			else
			{
				m_TextViewer.draw(deltaMilliseconds);
			}
		}
		ImGui::End();
	}
}
