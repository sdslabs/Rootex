#pragma once

#include "common/common.h"

class ImageResourceFile;
class CPUTexture;

/// Encapsulates all GPUTexture related functionalities, uses DirectXTK behind the scenes
class GPUTexture
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_MipLevels;

public:
	GPUTexture(const char* pixelData, int width, int height);
	GPUTexture(const char* imageFileData, size_t size);
	GPUTexture(const GPUTexture&) = default;
	GPUTexture& operator=(const GPUTexture&) = default;
	~GPUTexture() = default;

	ID3D11ShaderResourceView* getTextureResourceView() const { return m_TextureView.Get(); }
	ID3D11Texture2D* getD3D11Texture2D() const { return m_Texture.Get(); }
	unsigned int getWidth() const { return m_Width; }
	unsigned int getHeight() const { return m_Height; }
	unsigned int getMipLevels() const { return m_MipLevels; }
	unsigned char* download();
};

/// Texture accessible on CPU
class CPUTexture
{
	unsigned char* m_Buffer;
	unsigned int m_Width;
	unsigned int m_Height;

public:
	/// pixelData should contain values in RGBA format
	CPUTexture(unsigned char* pixelData, int width, int height);
	~CPUTexture();

	Color getPixel(int x, int y);
	void setPixel(int x, int y, Color color);
	unsigned int getWidth() const { return m_Width; }
	unsigned int getHeight() const { return m_Height; }

	const unsigned char* getBuffer() const { return m_Buffer; }
};

/// Cube GPUTexture in 3D
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
