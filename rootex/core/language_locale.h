#pragma once

#include "common/common.h"
/// Loads game strings from a .json file.\n
/// The expected format for the .json file consisting of game strings is {"key1":"game string 1" , "key2":"game string 2"}
class Locale
{
	HashMap<String, String> m_GameStrings;
	Locale() = default;

public:
	static Locale* GetSingleton();

	/// Used to get game string of the current language by passing the key defined for the string in the .json file
	String getString(const String key);
	/// Used to load game strings from the .json file present in the location passed
	void loadLanguage(const String location);
};
