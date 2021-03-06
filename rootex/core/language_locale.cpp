#include "core/language_locale.h"

Locale* Locale::GetSingleton()
{
	static Locale singleton;
	return &singleton;
}

void Locale::loadLanguage(const String location)
{
	if (OS::IsFile(location))
	{
		JSON::json file;
		std::ifstream i(location);
		i >> file;
		m_GameStrings = file.get<std::unordered_map<String, String>>();
	}
}

String Locale::getString(const String key)
{
	return m_GameStrings.at(key);
}
