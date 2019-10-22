#pragma once

#include <iostream>
#include <string>

#include "common/types.h"

void print(const String& msg);
void printLine(const String& msg);

void printWarning(const String& warning);
void printError(const String& error);
void printIf(const bool& expr, const String& error);

#ifdef _DEBUG
#define WARN(m_msg) printWarning(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define ERR(m_msg) printError(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define PANIC(m_if_true, m_msg) printIf(m_if_true, String(__FILE__) + ":" + String(__func__) + ": " + m_msg);

#else // TODO: Think about the behaviour in Release mode
#define WARN(m_msg) printWarning(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define ERR(m_msg) printError(String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#define PANIC(m_expr, m_msg) printIf(m_expr, String(__FILE__) + ":" + String(__func__) + ": " + m_msg);
#endif