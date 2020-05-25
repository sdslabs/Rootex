#pragma once

#include <d3d11.h>

class ImageResourceFile;

/// Encapsulates all Texture related functionalities, uses DirectXTK behind the scenes
class Texture
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
	ImageResourceFile* m_ImageFile;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_MipLevels;

	void loadTexture();

public:
	Texture(ImageResourceFile* imageFile);
	Texture(uint8_t* imageData, size_t size);
	Texture(Texture&) = delete;
	Texture& operator=(Texture&) = delete;
	~Texture() = default;

	void reload();

	ID3D11ShaderResourceView* getTextureResourceView() const { return m_TextureView.Get(); }
	ID3D11Texture2D* getD3D11Texture2D() const { return m_Texture.Get(); }
	unsigned int getWidth() const { return m_Width; }
	unsigned int getHeight() const { return m_Height; }
	unsigned int getMipLevels() const { return m_MipLevels; }
};
