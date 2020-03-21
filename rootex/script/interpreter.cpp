#include "interpreter.h"
#include "core/resource_loader.h"

#include "common/common.h"
#include "script/interpreter.h"
#include "entity.h"
#include "components/transform_component.h"

LuaInterpreter::LuaInterpreter()
{
	m_LuaState = luaL_newstate();
	luaopen_base(m_LuaState);
	luaL_openlibs(m_LuaState);

	loadExecuteScript(ResourceLoader::CreateLuaTextResourceFile("rootex/script/script_env.lua"));
}

LuaInterpreter::~LuaInterpreter()
{
	lua_close(m_LuaState);
}

LuaInterpreter* LuaInterpreter::GetSingleton()
{
	static LuaInterpreter singleton;
	return &singleton;
}

void LuaInterpreter::loadExecuteScript(LuaTextResourceFile* script)
{
	PANIC(script->getType() != ResourceFile::Type::Lua, "LuaInterpreter: Running non-Lua script source");

	bool error = luaL_dostring(m_LuaState, script->getString().c_str());
	OS::PrintLine(script->getPath().generic_string() + " was run");
	if (error)
	{
		ERR("Error occured in " + script->getPath().generic_string() + ": " + String(lua_tostring(m_LuaState, -1)));
	}
}

void LuaInterpreter::loadExecuteScript(const String& script)
{
	PANIC(script == "", "Lua inline script was found empty");
	bool error = luaL_dostring(m_LuaState, script.c_str());
	if (error)
	{
		ERR("Error occured in string script:\n" + script + "\nError: " + String(lua_tostring(m_LuaState, -1)));
	}
}

LuaVariable LuaInterpreter::createTable(const String& name)
{
	return luabridge::newTable(m_LuaState);
}

LuaVariable LuaInterpreter::getGlobal(const String& name)
{
	LuaVariable result = luabridge::getGlobal(m_LuaState, name.c_str());
	if (result.isNil())
	{
		ERR("Lua variable (" + name + ") was not found");
	}
	return result;
}

void LuaInterpreter::setGlobal(LuaVariable luaVar, const String& name)
{
	luabridge::setGlobal(m_LuaState, luaVar, name.c_str());
}

template<class T>
void BindClass()
{
	T::BindFunctions();
}

void LuaInterpreter::registerTypes()
{
	luabridge::getGlobalNamespace(m_LuaState)
	    .beginNamespace("Rootex")

		.beginClass<Vector2>("Vector2")
	    .addConstructor<void (*)(float x, float y)>()
	    .addData<float>("x", &Vector2::x)
	    .addData<float>("y", &Vector2::y)
	    .endClass()

		.beginClass<Vector3>("Vector3")
	    .addConstructor<void (*)(float x, float y, float z)>()
	    .addData<float>("x", &Vector3::x)
	    .addData<float>("y", &Vector3::y)
	    .addData<float>("z", &Vector3::z)
	    .endClass()

		.beginClass<Vector4>("Vector4")
	    .addConstructor<void (*)(float x, float y, float z, float w)>()
	    .addData<float>("x", &Vector4::x)
	    .addData<float>("y", &Vector4::y)
	    .addData<float>("z", &Vector4::z)
	    .addData<float>("w", &Vector4::w)
	    .endClass()

		.beginClass<Color>("Color")
	    .addConstructor<void (*)(float x, float y, float z, float w)>()
	    .addData<float>("x", &Color::x)
	    .addData<float>("y", &Color::y)
	    .addData<float>("z", &Color::z)
	    .addData<float>("w", &Color::w)
	    .endClass()

	    .beginClass<Quaternion>("Quaternion")
	    .addConstructor<void (*)(float x, float y, float z, float w)>()
	    .addData<float>("x", &Quaternion::x)
	    .addData<float>("y", &Quaternion::y)
	    .addData<float>("z", &Quaternion::z)
	    .addData<float>("w", &Quaternion::w)
		.endClass()
	    
		.endNamespace();

	BindClass<ResourceFile>();
	BindClass<TextResourceFile>();
	BindClass<LuaTextResourceFile>();
	BindClass<AudioResourceFile>();
	BindClass<VisualModelResourceFile>();
	BindClass<ImageResourceFile>();
	BindClass<FontResourceFile>();

	BindClass<ResourceLoader>();
	
	BindClass<Entity>();
	
	BindClass<Component>();
	BindClass<TransformComponent>();
}
