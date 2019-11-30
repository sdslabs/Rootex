#pragma once

#include "common/types.h"
#include "core/resource_file.h"
#include "os/os.h"

class ResourceData
{
	static unsigned int s_Count;

public:
	ResourceData(std::vector<char> data);
	~ResourceData() = default;

	unsigned int getID();
	String getPath();
	FileBuffer* getRawData();
	unsigned int getRawDataByteSize();

	void setPath(String path);

	ResourceData& operator>>(char& fillIn);
	operator bool() const;
	bool isEndOfFile();
	void resetStream();

protected:
	unsigned int m_ID;
	FileBuffer m_FileBuffer;
	String m_Path;

	char* m_StreamStart;
	const char* m_StreamEnd;
};
