#include "common.h"

void print(const String& msg)
{
	std::cout.clear();
    std::cout << msg;
}

void printLine(const String& msg)
{
	std::cout.clear();
    std::cout << msg << std::endl;
}

void printWarning(const String& warning)
{
    std::cout.clear();
    std::cout << "\033[93m" << warning << "\033[0m" << std::endl;
}

void printError(const String& error)
{
	std::cout.clear();
    std::cout << "\033[91m" << error << "\033[0m" << std::endl;
}

void printIf(const bool& expr, const String& error)
{
    if (expr) {
        printError(error);
    }
}
