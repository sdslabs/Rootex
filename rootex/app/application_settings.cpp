#include "application_settings.h"

ApplicationSettings* ApplicationSettings::s_Instance = nullptr;

ApplicationSettings::ApplicationSettings(Ref<TextResourceFile> settingsFile)
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
	PANIC(!m_TextSettingsFile->save(), "Could not save application settings file");
}

JSON::json::iterator ApplicationSettings::find(const String& setting)
{
	return m_Settings.find(setting);
}

JSON::json::iterator ApplicationSettings::end()
{
	return m_Settings.end();
}
