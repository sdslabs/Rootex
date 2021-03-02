#pragma once

#include "resource_file.h"

/// Representation of a text file.
class TextResourceFile : public ResourceFile
{
protected:
	String m_FileString;

	explicit TextResourceFile(const FilePath& path);

	friend class ResourceLoader;

public:
	explicit TextResourceFile(TextResourceFile&) = delete;
	explicit TextResourceFile(TextResourceFile&&) = delete;
	virtual ~TextResourceFile() = default;

	void reimport() override;
	bool save() override;

	/// Replace old data string with new data string.
	void putString(const String& newData);
	/// Remove 1 character from the end of the data buffer.
	void popBack();
	void append(const String& add);
	/// Get the resource data buffer as a readable String.
	String getString() const;
	size_t getSize() const { return m_FileString.size(); }
};
