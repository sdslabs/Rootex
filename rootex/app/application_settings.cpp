#include "application_settings.h"

#include "resource_loader.h"

ApplicationSettings* ApplicationSettings::s_Instance = nullptr;

ApplicationSettings::ApplicationSettings(TextResourceFile* settingsFile)
    : m_Settings(JSON::json::parse(settingsFile->getString()))
    , m_TextSettingsFile(settingsFile)
{
	if (!s_Instance)
	{
		s_Instance = this;
	}
	else
	{
		ERR("Multiple settings are active");
	}
}

ApplicationSettings::~ApplicationSettings()
{
	m_TextSettingsFile->putString(m_Settings.dump(4));
	ResourceLoader::SaveResourceFile(m_TextSettingsFile);
}

JSON::json::iterator ApplicationSettings::find(const String& setting)
{
	return m_Settings.find(setting);
}

JSON::json::iterator ApplicationSettings::end()
{
	return m_Settings.end();
}
