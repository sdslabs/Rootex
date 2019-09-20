#pragma once

#include <fstream>

class OS
{
public:
	static OS getSingleton();

	std::string loadFileContents(std::string path);
	bool exists(std::string path);
};
