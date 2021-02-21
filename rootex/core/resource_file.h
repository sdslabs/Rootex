#pragma once

#include "common/common.h"

/// Interface of a file loaded from disk. Use ResourceLoader to load, create or save files.
class ResourceFile
{
public:
	/// RTTI storage for the type of file being represented.
	enum class Type : int
	{
		/// Signifies an error in loading. Every valid ResourceFile will have a non-None type.
		None = 0,
		Lua,
		Audio,
		Text,
		Model,
		AnimatedModel,
		CollisionModel,
		Image,
		ImageCube,
		Font
	};

protected:
	Type m_Type;
	FilePath m_Path;
	FileTimePoint m_LastReadTime;
	FileTimePoint m_LastChangedTime;

	explicit ResourceFile(const Type& type, const FilePath& path);

	friend class ResourceLoader;

public:
	static void RegisterAPI(sol::table& rootex);

	explicit ResourceFile(const ResourceFile&) = delete;
	explicit ResourceFile(const ResourceFile&&) = delete;
	virtual ~ResourceFile() = default;

	/// Reload the file buffer from disk.
	virtual void reimport();
	virtual bool save();

	/// If the file has been changed on disk.
	bool isDirty();

	FilePath getPath() const;
	Type getType() const;
	const FileTimePoint& getLastReadTime() const { return m_LastReadTime; }
	const FileTimePoint& getLastChangedTime();
};
