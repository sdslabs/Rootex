#pragma once

#include "common/types.h"

class ResourceLoader;
class ResourceData;

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
	ResourceData* m_ResourceData;

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
public:
	String getText();

protected:
	explicit TextResource(const String& name, const String& path, const Type& type = Type::TXT);

	friend ResourceLoader;

};

class LuaScriptResource : public TextResource
{
	explicit LuaScriptResource(const String& name, const String& path);

	friend ResourceLoader;

public:
	String getSource();
};

class WAVResource : IResourceFile
{
	explicit WAVResource(const String& name, const String& path);

	friend ResourceLoader;

public:
};
