#pragma once

#include "common/types.h"
#include "core/resource_file.h"

class ResourceData
{
	static unsigned int s_Count;

public:
	ResourceData(std::vector<char> data);
	~ResourceData() = default;

	unsigned int getID();
	String getPath();
	std::vector<char>& getRawData();
	unsigned int getRawDataByteSize();

	void setPath(String path);

protected:
	unsigned int m_ID;
	Vector<char> m_Buffer;
	String m_Path;
};
