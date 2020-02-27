#pragma once

#include "common/types.h"
#include "os/os.h"

#ifdef _DEBUG
#define WARN(m_Msg) ::OS::PrintWarning(String(__FILE__) + ":" + String(__func__) + ": " + m_Msg);
#define ERR(m_Msg) ::OS::PrintError(String(__FILE__) + ":" + String(__func__) + ": " + m_Msg);
#define PANIC(m_if_true, m_Msg) ::OS::PrintIf(m_if_true, String(__FILE__) + ":" + String(__func__) + ": " + m_Msg);
#define ERR_CUSTOM(m_file, m_func, m_Msg) ::OS::PrintError(String(m_file) + ":" + String(m_func) + ": " + m_Msg);
#else // TODO: Think about the behaviour in Release mode
#define WARN(m_Msg) ::OS::PrintWarning(String(__FILE__) + ":" + String(__func__) + ": " + m_Msg);
#define ERR(m_Msg) ::OS::PrintError(String(__FILE__) + ":" + String(__func__) + ": " + m_Msg);
#define PANIC(m_expr, m_Msg) ::OS::PrintIf(m_expr, String(__FILE__) + ":" + String(__func__) + ": " + m_Msg);
#define ERR_CUSTOM(m_file, m_func, m_Msg) ::OS::PrintError(String(m_file) + ":" + String(m_func) + ": " + m_Msg);
#endif // _DEBUG
