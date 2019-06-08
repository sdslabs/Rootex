#include "Logger.h"

#include <iostream>
#include <string>
#include <varargs.h>

void Logger::Print(std::string &File, unsigned int &Line, std::string &FormatString, ...)
{
	m_Stream << File << ":" << Line << std::printf(FormatString.c_str(), ...);
}

Logger::Logger(const std::ostream & OutputStream)
	: m_Stream(OutputStream)
{
}
