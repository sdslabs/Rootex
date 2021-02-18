#include "lua_text_resource_file.h"

LuaTextResourceFile::LuaTextResourceFile(const FilePath& path)
    : TextResourceFile(path)
{
	m_Type = Type::Lua;
}
