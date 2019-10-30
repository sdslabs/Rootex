#pragma once

#include "common/types.h"
#include "core/resource_file.h"

class ResourceData
{
public:
	virtual const char* getRawData();
	virtual unsigned int getRawDataByteSize();

protected:
	const char* m_Buffer;
	unsigned int m_BufferByteSize;
	String m_Path;

	explicit ResourceData(const char* data, const unsigned int byteSize);
	~ResourceData();

	friend class ResourceManager;
	friend class ResourceLoader;
};
