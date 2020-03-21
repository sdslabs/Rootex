#pragma once

#include "common/types.h"
#include "core/resource_data.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/index_buffer.h"
#include "DirectXTK/Inc/SpriteFont.h"

class ResourceFile
{
public:
	enum class Type : int
	{
		None = 0,
		Lua,
		Wav,
		Text,
		Obj,
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
	static void BindFunctions();

	virtual ~ResourceFile();
	explicit ResourceFile(ResourceFile&) = delete;
	explicit ResourceFile(ResourceFile&&) = delete;

	bool isValid();
	bool isDirty();
	bool isOpen();

	virtual void reload();

	FilePath getPath() const;
	Type getType() const;
	ResourceData* getData();
	const FileTimePoint& getLastReadTime() const { return m_LastReadTime; }
	const FileTimePoint& getLastChangedTime();
};

class TextResourceFile : public ResourceFile
{
protected:
	explicit TextResourceFile(const Type& type, ResourceData* resData);
	virtual ~TextResourceFile();

	friend class ResourceLoader;

public:
	static void BindFunctions();
	
	explicit TextResourceFile(TextResourceFile&) = delete;
	explicit TextResourceFile(TextResourceFile&&) = delete;

	virtual void reload() override;

	void popBack();
	void append(const String& add);
	String getString() const;
};

class LuaTextResourceFile : public TextResourceFile
{
	explicit LuaTextResourceFile(ResourceData* resData);
	~LuaTextResourceFile();

	friend class ResourceLoader;

public:
	static void BindFunctions();

	explicit LuaTextResourceFile(TextResourceFile&) = delete;
	explicit LuaTextResourceFile(TextResourceFile&&) = delete;
};

typedef int ALsizei;
typedef int ALenum;

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
	static void BindFunctions();

	explicit AudioResourceFile(AudioResourceFile&) = delete;
	explicit AudioResourceFile(AudioResourceFile&&) = delete;

	virtual void reload() override;

	ALsizei getAudioDataSize() const { return m_AudioDataSize; }
	ALenum getFormat() const { return m_Format; }
	float getFrequency() const { return m_Frequency; }
	int getBitDepth() const { return m_BitDepth; }
	int getChannels() const { return m_Channels; }
	float getDuration() const { return m_Duration; }
};

class VisualModelResourceFile : public ResourceFile
{
	explicit VisualModelResourceFile(Ptr<VertexBuffer> vertexBuffer, Ptr<IndexBuffer> indexBuffer, ResourceData* resData);
	~VisualModelResourceFile();

	Ptr<VertexBuffer> m_VertexBuffer;
	Ptr<IndexBuffer> m_IndexBuffer;

	friend class ResourceLoader;

public:
	static void BindFunctions();

	explicit VisualModelResourceFile(VisualModelResourceFile&) = delete;
	explicit VisualModelResourceFile(VisualModelResourceFile&&) = delete;

	virtual void reload() override;

	const VertexBuffer* getVertexBuffer() const { return m_VertexBuffer.get(); }
	const IndexBuffer* getIndexBuffer() const { return m_IndexBuffer.get(); }
};

class ImageResourceFile : public ResourceFile
{
	explicit ImageResourceFile(ResourceData* resData);
	~ImageResourceFile();

	friend class ResourceLoader;

public:
	static void BindFunctions();

	explicit ImageResourceFile(ImageResourceFile&) = delete;
	explicit ImageResourceFile(ImageResourceFile&&) = delete;

	virtual void reload() override;
};

class FontResourceFile : public ResourceFile
{
	explicit FontResourceFile(const String& name, ResourceData* resData);
	~FontResourceFile();

	String m_Name;
	Ref<DirectX::SpriteFont> m_Font;

	friend class ResourceLoader;

public:
	static void BindFunctions();

	explicit FontResourceFile(ImageResourceFile&) = delete;
	explicit FontResourceFile(ImageResourceFile&&) = delete;

	virtual void reload() override;

	String getFontName() const { return m_Name; }
	Ref<DirectX::SpriteFont> getFont() const { return m_Font; }
};
