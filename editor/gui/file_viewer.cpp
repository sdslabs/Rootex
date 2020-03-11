#include "file_viewer.h"

#include "resource_loader.h"

#include "imgui.h"

Variant FileViewer::openFile(const Event* event)
{
	m_IsFileOpened = true;
	m_IsEventJustReceived = true;

	m_AudioPlayer.unload();
	m_ImageViewer.unload();
	m_TextViewer.unload();

	m_OpenFilePath = Extract(FilePath, event->getData());

	const String& ext = m_OpenFilePath.extension().string();
	if (ext == ".wav")
	{
		m_AudioPlayer.load(m_OpenFilePath);
	}
	else if (ext == ".jpg" || ext == ".png" || ext == ".jpeg")
	{
		m_ImageViewer.load(m_OpenFilePath);
	}
	else
	{
		m_TextViewer.load(m_OpenFilePath);
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

void FileViewer::draw()
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
			if (m_OpenFilePath.extension() == ".wav")
			{
				m_AudioPlayer.draw();
			}
			else if (m_OpenFilePath.extension() == ".png" || m_OpenFilePath.extension() == ".jpg" || m_OpenFilePath.extension() == ".jpeg")
			{
				m_ImageViewer.draw();
			}
			else
			{
				m_TextViewer.draw();
			}
		}
		ImGui::End();
	}
}
