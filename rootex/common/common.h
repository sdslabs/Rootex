#pragma once

#include <iostream>
#include <string>

#include "common/types.h"
#include "os/os.h"

#ifdef _DEBUG
#define WARN(m_msg) ::OS::PrintWarning(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define ERR(m_msg) ::OS::PrintError(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define PANIC(m_if_true, m_msg) ::OS::PrintIf(m_if_true, String(__FILE__) + ":" + String(__func__) + ": " + m_msg);

#else // TODO: Think about the behaviour in Release mode
#define WARN(m_msg) ::OS::printWarning(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define ERR(m_msg) ::OS::printError(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define PANIC(m_expr, m_msg) ::OS::printIf(m_expr, String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#endif