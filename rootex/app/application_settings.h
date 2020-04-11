#pragma once

#include "common/common.h"

#include "resource_file.h"

class ApplicationSettings
{
	static ApplicationSettings* s_Instance;
	TextResourceFile* m_TextSettingsFile;
	JSON::json m_Settings;

public:
	ApplicationSettings(TextResourceFile* settingsFile);
	ApplicationSettings(ApplicationSettings&) = delete;
	~ApplicationSettings();

	JSON::json::iterator find(const String& setting);
	JSON::json::iterator end();

	JSON::json& getJSON() { return m_Settings; };
};
