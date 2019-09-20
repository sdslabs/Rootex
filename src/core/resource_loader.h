#pragma once

#include <string>
#include "resource.h"

class ResourceLoader 
{
public:
	static TextFile* createFileResource(std::string path);
};
