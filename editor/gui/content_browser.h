#pragma once

#include "scene.h"

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

public:
	ContentBrowser();
	ContentBrowser(const ContentBrowser&) = delete;
	~ContentBrowser() = default;

	void draw(float deltaMilliseconds);

	ContentBrowserSettings& getSettings() { return m_ContentBrowserSettings; }
	void setActive(bool enabled) { m_ContentBrowserSettings.m_IsActive = enabled; }
};
