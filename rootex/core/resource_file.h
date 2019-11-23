#pragma once

#include "common/types.h"

class ResourceData;

class ResourceFile
{
public:
	enum class Type : int
	{
		NONE = 0,
		LUA,
		WAV,
		TXT
	};

protected:
	String m_Name;
	String m_Path;
	Type m_Type;
	ResourceData* m_ResourceData;

public:
	explicit ResourceFile(const String& name, const String& path, const Type& type);
	explicit ResourceFile(ResourceFile&) = delete;
	explicit ResourceFile(ResourceFile&&) = delete;
	~ResourceFile();

	bool isValid();
	bool isOpen();

	String getName();
	String getPath();
	Type getType();
	ResourceData* getData();
	String getDataString();
	String getText();

	void setResourceData(ResourceData* resourceData);
};
