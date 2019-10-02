#pragma once

#include <string>
#include <vector>

class ResourceLoader;

class Resource 
{
protected:

	std::string m_Path;

	Resource(const std::string& path);
	Resource(Resource&) = delete;
	Resource(Resource&& oldResource);

	friend class ResourceLoader;

public:
	virtual ~Resource();

	bool isValid();
	std::string getPath();
};

class TextFile : public Resource
{
protected:
	std::string m_Data;

public:
	TextFile() = delete;
	TextFile(const std::string& path, std::string& contents);
	TextFile(TextFile&) = delete;
	TextFile(TextFile&& oldFile);
	virtual ~TextFile();

	std::string getContents();
};

class Script : public TextFile
{
	std::string m_Type;
	
	Script(const std::string& path, std::string& contents);

	friend ResourceLoader;

public:
	std::string getType() const { return m_Type; }
};
