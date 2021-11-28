#pragma once

#include "scene.h"
#include "resource_loader.h"
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
	const std::filesystem::path m_AssetsDirectory {"game\\assets"};
	std::filesystem::path m_CurrentDirectory { m_AssetsDirectory };
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
