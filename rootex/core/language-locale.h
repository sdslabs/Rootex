#pragma once

#include "common/common.h"

class Locale
{
	std::unordered_map<String, String> m_GameStrings;

	Locale();
	~Locale();

public:
	String GetString(String key);
	void LoadLanguage(String location);
	static Locale* GetSingleton();
};
