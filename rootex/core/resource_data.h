#pragma once

#include "common/types.h"
#include "core/resource_file.h"

class ResourceData
{
	static unsigned int s_Count;

public:
	ResourceData(std::vector<char> data);
	~ResourceData();

	unsigned int getID();
	std::vector<char> getRawData();
	unsigned int getRawDataByteSize();

protected:
	unsigned int m_ID;
	Vector<char> m_Buffer;
	String m_Path;

	friend class ResourceManager;
	friend class ResourceLoader;
};
