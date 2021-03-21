#pragma once

#include "common/common.h"

#include "core/resource_files/text_resource_file.h"

class ApplicationSettings
{
	static ApplicationSettings* s_Instance;
	Ref<TextResourceFile> m_TextSettingsFile;
	JSON::json m_Settings;

public:
	static ApplicationSettings* GetSingleton() { return s_Instance; }

	ApplicationSettings(Ref<TextResourceFile> settingsFile);
	ApplicationSettings(ApplicationSettings&) = delete;
	~ApplicationSettings();

	JSON::json::iterator find(const String& setting);
	JSON::json::iterator end();

	TextResourceFile* getTextFile() const { return m_TextSettingsFile.get(); }
	JSON::json& getJSON() { return m_Settings; };
};
