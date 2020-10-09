#pragma once

#include "resource_file.h"

#include "SpriteFont.h"

/// Representation of a font file. Supports .spritefont files.
class FontResourceFile : public ResourceFile
{
	explicit FontResourceFile(const FilePath& path);

	Ref<DirectX::SpriteFont> m_Font;

	friend class ResourceLoader;

public:
	static void RegisterAPI(sol::table& rootex);

	explicit FontResourceFile(const FontResourceFile&) = delete;
	explicit FontResourceFile(const FontResourceFile&&) = delete;
	~FontResourceFile() = default;

	void reimport() override;

	Ref<DirectX::SpriteFont> getFont() const { return m_Font; }
};
