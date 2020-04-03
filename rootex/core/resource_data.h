#pragma once

#include "common/common.h"
#include "os/os.h"

#define KB_TO_B (1024.0f)
#define B_TO_KB (1.0f / KB_TO_B)
#define MB_TO_KB 1024.0f
#define KB_TO_MB (1.0f / MB_TO_KB)
#define GB_TO_MB 1024.0f
#define MB_TO_GB (1.0f / GB_TO_MB)

class ResourceData
{
	static unsigned int s_Count;

protected:
	unsigned int m_ID;
	FileBuffer m_FileBuffer;
	FilePath m_Path;

	char* m_StreamStart;
	const char* m_StreamEnd;

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
	void startStream();
	bool isEndOfFile();
	void resetStream();
};
