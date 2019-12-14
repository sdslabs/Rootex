#pragma once

#include "common/types.h"
#include "core/resource_file.h"
#include "os/os.h"

class ResourceData
{
	static unsigned int s_Count;

public:
	ResourceData(FilePath path, FileBuffer& data);
	~ResourceData() = default;

	unsigned int getID();
	FilePath getPath();
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
	FilePath m_Path;

	char* m_StreamStart;
	const char* m_StreamEnd;
};
