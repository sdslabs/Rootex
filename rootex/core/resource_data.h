#pragma once

#include "common/common.h"
#include "os/os.h"

/// Convert kilobytes to bytes
#define KB_TO_B (1024.0f)
/// Convert bytes to kilobytes
#define B_TO_KB (1.0f / KB_TO_B)
/// Convert megabytes to kilobytes
#define MB_TO_KB 1024.0f
/// Convert kilobytes to megabytes
#define KB_TO_MB (1.0f / MB_TO_KB)
/// Convert gigabytes to megabytes
#define GB_TO_MB 1024.0f
/// Convert megabytes to gigabytes
#define MB_TO_GB (1.0f / GB_TO_MB)

/// Representation of a ResourceFile data buffer. Contains a faceless collection of bytes loaded from disk.
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
	/// Get the collection of bytes in a file
	FileBuffer* getRawData();
	/// Get the number of bytes in a file
	unsigned int getRawDataByteSize();

	/// Set the path of file loaded. Potentially dangerous to use if you don't know what gets effected.
	void setPath(String path);

	/// Stream-like operator to get the file data byte-by-byte
	ResourceData& operator>>(char& fillIn);
	/// Returns false if data is empty
	operator bool() const;
	/// Begin stream-like access
	void startStream();
	/// If stream-like access has reached the end of file
	bool isEndOfFile();
	/// Reset the stream pointers on the buffer. Stream-like access gets set to the beginning of the file data.
	void resetStream();
};
