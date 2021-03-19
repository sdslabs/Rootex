#pragma once

#include "common.h"

#include "vendor/Lua/src/lua.hpp"
#define SOL_ALL_SAFETIES_ON 1
#define SOL_USING_CXX_LUA 1
#define SOL_PRINT_ERRORS 1

#if (defined(__cplusplus) && __cplusplus >= 201703L) \
    || (defined(_MSC_VER) && _MSC_VER > 1900 && ((defined(_HAS_CXX17) && _HAS_CXX17 == 1) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201402L))))
#endif // C++17 features check
#if defined(__cpp_noexcept_function_type) || ((defined(_MSC_VER) && _MSC_VER > 1911) && (defined(_MSVC_LANG) && ((_MSVC_LANG >= 201403L))))
#ifndef SOL_NOEXCEPT_FUNCTION_TYPE
#define SOL_NOEXCEPT_FUNCTION_TYPE 1
#endif // noexcept is part of a function's type
#endif // compiler-specific checks
#if defined(__clang__) && defined(__APPLE__)
#if defined(__has_include)
#if __has_include(<variant>)
#define SOL_STD_VARIANT 1
#endif // has include nonsense
#endif // __has_include
#else
#define SOL_STD_VARIANT 1
#endif // Clang screws up variant

#include "sol/sol.hpp"

/// Lua interpreter that runs all Lua scripts inside the same Lua state. This means that all Lua code-snippets that are run can cross-reference each other.
class LuaInterpreter
{
	sol::state m_Lua;

	LuaInterpreter();
	LuaInterpreter(LuaInterpreter&) = delete;
	~LuaInterpreter() = default;

	void registerTypes();
	void runScripts();

public:
	static LuaInterpreter* GetSingleton();

	sol::state& getLuaState() { return m_Lua; }
};
