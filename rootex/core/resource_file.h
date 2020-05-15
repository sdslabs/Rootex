#pragma once

#include "common/common.h"
#include "core/resource_data.h"
#include "core/renderer/mesh.h"
#include "core/renderer/texture.h"
#include "DirectXTK/Inc/SpriteFont.h"

/// Interface of a file loaded from disk. Use ResourceLoader to load, create or save files.
class ResourceFile
{
public:
	/// RTTI storage for the type of file being represented.
	enum class Type : int
	{
		/// Signifies an error in loading. Every ResourceFile will have a non-None type.
		None = 0,
		Lua,
		Audio,
		Text,
		Model,
		Image,
		Font
	};

protected:
	Type m_Type;
	ResourceData* m_ResourceData;
	FileTimePoint m_LastReadTime;
	FileTimePoint m_LastChangedTime;

	explicit ResourceFile(const Type& type, ResourceData* resData);

	friend class ResourceLoader;

public:
	virtual ~ResourceFile();
	explicit ResourceFile(ResourceFile&) = delete;
	explicit ResourceFile(ResourceFile&&) = delete;

	/// If the file was correctly loaded and set up. Should not return false without ResourceLoader showing an error.
	bool isValid();
	/// If the file has been changed on disk.
	bool isDirty();
	/// If the file data has been loaded properly.
	bool isOpen();

	FilePath getPath() const;
	Type getType() const;
	ResourceData* getData();
	const FileTimePoint& getLastReadTime() const { return m_LastReadTime; }
	const FileTimePoint& getLastChangedTime();
};

/// Representation of a text file.
class TextResourceFile : public ResourceFile
{
protected:
	explicit TextResourceFile(const Type& type, ResourceData* resData);
	virtual ~TextResourceFile();

	friend class ResourceLoader;

public:
	explicit TextResourceFile(TextResourceFile&) = delete;
	explicit TextResourceFile(TextResourceFile&&) = delete;

	/// Replace old data string with new data string.
	void putString(const String& newData);
	/// Remove 1 character from the end of the data buffer.
	void popBack();
	void append(const String& add);
	/// Get the resource data buffer as a readable String.
	String getString() const;
};

/// Representation of a text file that has Lua code.
class LuaTextResourceFile : public TextResourceFile
{
	explicit LuaTextResourceFile(ResourceData* resData);
	~LuaTextResourceFile();

	friend class ResourceLoader;

public:
	explicit LuaTextResourceFile(TextResourceFile&) = delete;
	explicit LuaTextResourceFile(TextResourceFile&&) = delete;
};

typedef int ALsizei;
typedef int ALenum;

/// Representation of an audio file. Only .wav files are supported.
class AudioResourceFile : public ResourceFile
{
	ALenum m_Format;
	float m_Frequency;
	int m_BitDepth;
	int m_Channels;
	float m_Duration;

	const char* m_DecompressedAudioBuffer;
	ALsizei m_AudioDataSize;

	explicit AudioResourceFile(ResourceData* resData);
	~AudioResourceFile();

	friend class ResourceLoader;

public:
	explicit AudioResourceFile(AudioResourceFile&) = delete;
	explicit AudioResourceFile(AudioResourceFile&&) = delete;

	/// Get size of decompressed audio data.
	ALsizei getAudioDataSize() const { return m_AudioDataSize; }
	/// Returns the same enum value that OpenAL uses.
	ALenum getFormat() const { return m_Format; }
	float getFrequency() const { return m_Frequency; }
	int getBitDepth() const { return m_BitDepth; }
	int getChannels() const { return m_Channels; }
	/// Get the duration of the audio in seconds.
	float getDuration() const { return m_Duration; }
};

/// Representation of a 3D model file. Only .obj with .mtl files are supported.
class ModelResourceFile : public ResourceFile
{
	explicit ModelResourceFile(ResourceData* resData);
	~ModelResourceFile();

	Vector<Mesh> m_Meshes;
	Vector<Ref<Texture>> m_Textures;

	friend class ResourceLoader;

public:
	explicit ModelResourceFile(ModelResourceFile&) = delete;
	explicit ModelResourceFile(ModelResourceFile&&) = delete;

	Vector<Mesh>& getMeshes() { return m_Meshes; }
};

/// Representation of an image file. Supports BMP, JPEG, PNG, TIFF, GIF, HD Photo, or other WIC supported file containers
class ImageResourceFile : public ResourceFile
{
	explicit ImageResourceFile(ResourceData* resData);
	~ImageResourceFile();

	friend class ResourceLoader;

public:
	explicit ImageResourceFile(ImageResourceFile&) = delete;
	explicit ImageResourceFile(ImageResourceFile&&) = delete;
};

/// Representation of a font file. Supports .spritefont files.
class FontResourceFile : public ResourceFile
{
	explicit FontResourceFile(ResourceData* resData);
	~FontResourceFile();

	Ref<DirectX::SpriteFont> m_Font;

	friend class ResourceLoader;

	void regenerateFont();

public:
	explicit FontResourceFile(ImageResourceFile&) = delete;
	explicit FontResourceFile(ImageResourceFile&&) = delete;

	Ref<DirectX::SpriteFont> getFont() const { return m_Font; }
};
