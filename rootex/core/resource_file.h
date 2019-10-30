#pragma once

#include <string>
#include <vector>

#include "common/types.h"
#include "core/resource_data.h"

class ResourceLoader;

class IResourceFile
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
	Ref<ResourceData> m_ResourceData;

	explicit IResourceFile(const String& name, const String& path, const Type& type);
	explicit IResourceFile(IResourceFile&) = delete;
	explicit IResourceFile(IResourceFile&&) = delete;
	
	friend class ResourceLoader;

public:
	virtual ~IResourceFile();

	bool isValid();
	bool isOpen();

	String getName();
	String getPath();
	Type getType();
	ResourceData* getData();
};

class TextResource : public IResourceFile
{
	explicit TextResource(const String& name, const String& path);

	friend ResourceLoader;

public:
};

class LuaScriptResource : public IResourceFile
{
	explicit LuaScriptResource(const String& name, const String& path);

	friend ResourceLoader;

public:
};

class WAVResource : IResourceFile
{
	explicit WAVResource(const String& name, const String& path);

	friend ResourceLoader;

public:
};
