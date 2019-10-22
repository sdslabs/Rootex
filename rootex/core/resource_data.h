#pragma once

#include "common/types.h"
#include "core/resource.h"

class IResourceData
{
public:
	virtual const char* getRawData();
	virtual unsigned int getRawDataByteSize();

	template <class T>
	Ref<T> getData();

protected:
	const char* m_Buffer;
	unsigned int m_BufferByteSize;

	explicit IResourceData(const char* data, const unsigned int byteSize);
};

class TextFileData : public IResourceData
{
	String m_Text;

	explicit TextFileData(String contents);

public:
	String getData<String>();
	unsigned int getSize();
};

class LuaScriptData : IResourceData
{
	String m_Source;

	explicit LuaScriptData(String source);

public:
	String getData<String>();
	unsigned int getSize();

	String getSource();
};
