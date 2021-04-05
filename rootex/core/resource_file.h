#pragma once

#include "common/types.h"
#include "os/os.h"

/// Interface of a file loaded from disk. Use ResourceLoader to load, create or save files.
class ResourceFile
{
public:
	/// RTTI storage for the type of file being represented.
	enum class Type : int
	{
		/// Signifies an error in loading. Every valid ResourceFile will have a non-None type.
		None = 0,
		Lua = 1,
		Audio = 2,
		Text = 3,
		Model = 4,
		AnimatedModel = 5,
		CollisionModel = 6,
		Image = 7,
		ImageCube = 8,
		Font = 9,
		ParticleEffect = 10,
		BasicMaterial = 11,
		InstancingBasicMaterial = 12,
		AnimatedBasicMaterial = 13,
		SkyMaterial = 14
	};

	static const inline Map<Type, const String> s_TypeNames = {
		{ Type::None, "None" },
		{ Type::Lua, "Lua" },
		{ Type::Audio, "Audio" },
		{ Type::Text, "Text" },
		{ Type::Model, "Model" },
		{ Type::AnimatedModel, "AnimatedModel" },
		{ Type::CollisionModel, "CollisionModel" },
		{ Type::Image, "Image" },
		{ Type::ImageCube, "ImageCube" },
		{ Type::Font, "Font" },
		{ Type::ParticleEffect, "ParticleEffect" },
		{ Type::BasicMaterial, "BasicMaterial" },
		{ Type::InstancingBasicMaterial, "InstancingBasicMaterial" },
		{ Type::AnimatedBasicMaterial, "AnimatedBasicMaterial" },
		{ Type::SkyMaterial, "SkyMaterial" }
	};

	Type m_Type;
	FilePath m_Path;
	FileTimePoint m_LastReadTime;
	FileTimePoint m_LastChangedTime;

protected:
	explicit ResourceFile(const Type& type, const FilePath& path);

	friend class ResourceLoader;

public:
	explicit ResourceFile(const ResourceFile&) = delete;
	explicit ResourceFile(const ResourceFile&&) = delete;
	virtual ~ResourceFile() = default;

	/// Reload the file buffer from disk.
	virtual void reimport();
	virtual bool save();
	virtual void draw();

	/// If the file has been changed on disk.
	bool isDirty();

	FilePath getPath() const;
	Type getType() const;
	const FileTimePoint& getLastReadTime() const { return m_LastReadTime; }
	const FileTimePoint& getLastChangedTime();
};

typedef Vector<Pair<ResourceFile::Type, String>> ResourceCollection;

void to_json(JSON::json& j, const ResourceFile::Type& t);
void from_json(const JSON::json& j, ResourceFile::Type& s);
