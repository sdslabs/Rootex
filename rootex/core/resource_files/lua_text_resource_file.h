#pragma once

#include "text_resource_file.h"

/// Representation of a text file that has Lua code.
class LuaTextResourceFile : public TextResourceFile
{
	explicit LuaTextResourceFile(const FilePath& path);

	friend class ResourceLoader;

public:
	explicit LuaTextResourceFile(TextResourceFile&) = delete;
	explicit LuaTextResourceFile(TextResourceFile&&) = delete;
	~LuaTextResourceFile() = default;
};