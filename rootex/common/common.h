#pragma once

#include "common/types.h"
#include "os/os.h"
#include "script/interpreter.h"

/// Logs function, message in white color
#define PRINT(m_Msg)                                      \
	{                                                     \
		String msg = String(__FUNCTION__) + ": " + m_Msg; \
		::OS::Print(msg);                                 \
	}
/// Logs file, line, function, message in yellow color
#define WARN(m_Msg)                                                                                                 \
	{                                                                                                               \
		String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintWarning(msg);                                                                                    \
	}
/// Logs file, line, function, message in red color
#define ERR(m_Msg)                                                                                                  \
	{                                                                                                               \
		String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintError(msg);                                                                                      \
	}
/// Logs file, line, function, message in yellow color in condition is true
#define PANIC(m_IfTtrue, m_Msg) \
	if (m_IfTtrue)              \
	{                           \
		WARN(m_Msg);            \
	}
/// Panic, but only in debug mode
#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_PANIC(m_IfTtrue, m_Msg) \
	if (m_IfTtrue)                    \
	{                                 \
		WARN(m_Msg);                  \
	}
#else
#define DEBUG_PANIC(m_IfTtrue, m_Msg)
#endif
/// Logs file, function, message in red color
#define ERR_CUSTOM(m_file, m_func, m_Msg)                                          \
	{                                                                              \
		String msg = String(__FILE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintError(String(m_file) + ":" + String(m_func) + ": " + m_Msg);    \
	}

/// Logs function, message in white color, doesn't log in editor console
#define PRINT_SILENT(m_Msg)                                      \
	{                                                     \
		String msg = String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintSilent(msg);                                 \
	}
/// Logs file, line, function, message in yellow color, doesn't log in editor console
#define WARN_SILENT(m_Msg)                                                                                                 \
	{                                                                                                               \
		String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintWarningSilent(msg);                                                                                    \
	}
/// Logs file, line, function, message in red color, doesn't log in editor console
#define ERR_SILENT(m_Msg)                                                                                                  \
	{                                                                                                               \
		String msg = String(__FILE__) + ":" + std::to_string(__LINE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintErrorSilent(msg);                                                                                      \
	}
/// Logs file, line, function, message in yellow color in condition is true, doesn't log in editor console
#define PANIC_SILENT(m_IfTtrue, m_Msg) \
	if (m_IfTtrue)              \
	{                           \
		WARN_SILENT(m_Msg);            \
	}
/// Logs file, function, message in red color, doesn't log in editor console
#define ERR_CUSTOM_SILENT(m_file, m_func, m_Msg)                                          \
	{                                                                              \
		String msg = String(__FILE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintErrorSilent(String(m_file) + ":" + String(m_func) + ": " + m_Msg);    \
	}
