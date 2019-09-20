#include "common.h"

void print(const std::string& msg)
{
	std::cout.clear();
    std::cout << msg;
}

void printLine(const std::string& msg)
{
	std::cout.clear();
    std::cout << msg << std::endl;
}

void printWarning(const std::string& warning)
{
    std::cout.clear();
    std::cout << "\033[93m" << warning << "\033[0m" << std::endl;
}

void printError(const std::string& error)
{
	std::cout.clear();
    std::cout << "\033[91m" << error << "\033[0m" << std::endl;
}

void printIf(const bool& expr, const std::string& error)
{
    if (expr) {
        printError(error);
    }
}
