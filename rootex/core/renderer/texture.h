#pragma once

#include "common/common.h"

class ImageResourceFile;

/// Encapsulates all Texture related functionalities, uses DirectXTK behind the scenes
class Texture
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_MipLevels;

public:
	Texture(const char* pixelData, int width, int height);
	Texture(const char* imageFileData, size_t size);
	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;
	~Texture() = default;

	ID3D11ShaderResourceView* getTextureResourceView() const { return m_TextureView.Get(); }
	ID3D11Texture2D* getD3D11Texture2D() const { return m_Texture.Get(); }
	unsigned int getWidth() const { return m_Width; }
	unsigned int getHeight() const { return m_Height; }
	unsigned int getMipLevels() const { return m_MipLevels; }
};

/// Cube texture in 3D
class TextureCube
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
	
public:
	TextureCube(const char* imageDDSFileData, size_t size);
	TextureCube(const TextureCube&) = delete;
	TextureCube& operator=(const TextureCube&) = delete;
	~TextureCube() = default;

	ID3D11ShaderResourceView* getTextureResourceView() const { return m_TextureView.Get(); }
};
