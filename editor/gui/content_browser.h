#pragma once

#include "scene.h"
#include "resource_loader.h"
#include <os.h>

class ContentBrowser
{
	EventBinder<ContentBrowser> m_Binder;

public:
	struct ContentBrowserSettings
	{
		bool m_IsActive = true;
	};

private:
	ContentBrowserSettings m_ContentBrowserSettings;
	String m_AssetsDirectory = "game\\assets";
	String m_CurrentDirectory = m_AssetsDirectory;
	bool m_ReloadPending = true;

	Ref<ImageResourceFile> m_DirectoryImage;
	Ref<ImageResourceFile> m_ScriptImage;
	Ref<ImageResourceFile> m_MusicImage;

	float m_IconWidth = 128;

public:
	ContentBrowser();
	ContentBrowser(const ContentBrowser&) = delete;
	~ContentBrowser() = default;

	void draw(float deltaMilliseconds);

	ContentBrowserSettings& getSettings() { return m_ContentBrowserSettings; }
	void setActive(bool enabled) { m_ContentBrowserSettings.m_IsActive = enabled; }
};
