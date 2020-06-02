#include "texture.h"

#include "rendering_device.h"
#include "resource_loader.h"

Texture::Texture(ImageResourceFile* imageFile)
    : m_ImageFile(imageFile)
{
	loadTexture();
}

Texture::Texture(const char* imageData, int width, int height)
{
	m_TextureView = RenderingDevice::GetSingleton()->createTextureFromPixels(imageData, width, height);

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
    : m_ImageFile(nullptr)
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

void Texture::reload()
{
	m_TextureView.Reset();
	if (m_ImageFile)
	{
		ResourceLoader::Reload(m_ImageFile);
		loadTexture();
	}
	else
	{
		WARN("Cannot reload texture made from raw data");
	}
}

void Texture::loadTexture()
{
	m_TextureView = RenderingDevice::GetSingleton()->createTexture(m_ImageFile);

	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	m_TextureView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&m_Texture);

	CD3D11_TEXTURE2D_DESC textureDesc;
	m_Texture->GetDesc(&textureDesc);

	m_Width = textureDesc.Width;
	m_Height = textureDesc.Height;
	m_MipLevels = textureDesc.MipLevels;
}
