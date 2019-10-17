#pragma once

#include <string>
#include <vector>

#include "common/types.h"

class ResourceLoader;

class IResource 
{
protected:

	String m_Path;

	IResource(const String& path);
	IResource(IResource&) = delete;
	IResource(IResource&& oldResource);

	friend class ResourceLoader;

public:
	virtual ~IResource();

	bool isValid();
	String getPath();
	const char* getPathCStyle();
};

class IResourcePointer
{
	IResource* m_Resource;

public:
	IResourcePointer();
	~IResourcePointer();

	IResource* getResource() const;
};

class TextFile : public IResource
{
protected:
	String m_Data;

public:
	TextFile() = delete;
	TextFile(const String& path, String& contents);
	TextFile(TextFile&) = delete;
	TextFile(TextFile&& oldFile);
	virtual ~TextFile();

	String getContents() const;
};

class LuaScript : public TextFile
{	
	LuaScript(const String& path, String& contents);
	
	friend ResourceLoader;

public:
	String getSource() const { return getContents(); }
};

class SoundResource : IResource
{

};
