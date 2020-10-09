#include "lua_text_resource_file.h"

LuaTextResourceFile::LuaTextResourceFile(const FilePath& path)
    : TextResourceFile(path)
{
	m_Type = Type::Lua;
}

void LuaTextResourceFile::RegisterAPI(sol::table& rootex)
{
	sol::usertype<LuaTextResourceFile> luaTextResourceFile = rootex.new_usertype<LuaTextResourceFile>(
	    "LuaTextResourceFile",
	    sol::base_classes, sol::bases<ResourceFile, TextResourceFile>());
}
