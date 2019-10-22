#pragma once

#include <string>
#include <vector>

#include "common/types.h"

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

	explicit IResourceFile(const String& name, const String& path, const Type& type);
	explicit IResourceFile(IResourceFile&) = delete;
	explicit IResourceFile(IResourceFile&&) = delete;

	friend class ResourceLoader;

public:
	virtual ~IResourceFile();

	virtual bool isValid() = 0;
	virtual bool open() = 0;

	String getName();
	String getPath();
	Type getType();
};

class TextFile : public IResourceFile
{
	explicit TextFile(const String& name, const String& path, const Type& type);

	friend ResourceLoader;

public:
};

class LuaScriptResource : public IResourceFile
{
	explicit LuaScriptResource(const String& name, const String& path, const Type& type);

	friend ResourceLoader;

public:
};

class WAVResource : IResourceFile
{
	explicit WAVResource(const String& name, const String& path, const Type& type);

	friend ResourceLoader;

public:
};
