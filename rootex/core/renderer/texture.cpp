#include "texture.h"

#include "rendering_device.h"
#include "resource_loader.h"

Texture::Texture(const char* pixelData, int width, int height)
{
	m_TextureView = RenderingDevice::GetSingleton()->createTextureFromPixels(pixelData, width, height);

	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	m_TextureView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&m_Texture);

	CD3D11_TEXTURE2D_DESC textureDesc;
	m_Texture->GetDesc(&textureDesc);

	m_Width = textureDesc.Width;
	m_Height = textureDesc.Height;
	m_MipLevels = textureDesc.MipLevels;
}

Texture::Texture(const char* imageFileData, size_t size)
{
	m_TextureView = RenderingDevice::GetSingleton()->createTexture(imageFileData, size);

	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	m_TextureView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&m_Texture);

	CD3D11_TEXTURE2D_DESC textureDesc;
	m_Texture->GetDesc(&textureDesc);

	m_Width = textureDesc.Width;
	m_Height = textureDesc.Height;
	m_MipLevels = textureDesc.MipLevels;
}

TextureCube::TextureCube(const char* imageDDSFileData, size_t size)
{
	m_TextureView = RenderingDevice::GetSingleton()->createDDSTexture(imageDDSFileData, size);
}
