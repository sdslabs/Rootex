#pragma once

#include "common/common.h"

struct ThemeDefinition
{
	String m_Name;
	String m_filePath;
	JSON::json m_ThemeData;

	void apply() const;
};
