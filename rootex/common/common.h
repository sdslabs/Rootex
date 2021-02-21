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
/// Logs file, function, message in red color
#define ERR_CUSTOM(m_file, m_func, m_Msg)                                          \
	{                                                                              \
		String msg = String(__FILE__) + ":" + String(__FUNCTION__) + ": " + m_Msg; \
		::OS::PrintError(String(m_file) + ":" + String(m_func) + ": " + m_Msg);    \
	}
