#include <iostream>
#include <string>

void print(const std::string& msg);
void printLine(const std::string& msg);

void printWarning(const std::string& warning);
void printError(const std::string& error);
void printIf(const bool& expr, const std::string& error);

#ifdef _DEBUG
#define WARN(m_msg) printWarning(std::string(__FILE__) + ":" + std::string(__FUNCSIG__) + ": " + m_msg);
#define ERR(m_msg) printError(std::string(__FILE__) + ":" + std::string(__FUNCSIG__) + ": " + m_msg);
#define PANIC(m_if_true, m_msg) printIf(m_if_true, std::string(__FILE__) + ":" + std::string(__FUNCSIG__) + ": " + m_msg);

#else // TODO: Think about the behaviour in Release mode
#define WARN(m_msg) printWarning(std::string(__FILE__) + ":" + std::string(__FUNCSIG__) + ": " + m_msg);
#define ERR(m_msg) printError(std::string(__FILE__) + ":" + std::string(__FUNCSIG__) + ": " + m_msg);
#define PANIC(m_expr, m_msg) printIf(m_expr, std::string(__FILE__) + ":" + std::string(__FUNCSIG__) + ": " + m_msg);
#endif