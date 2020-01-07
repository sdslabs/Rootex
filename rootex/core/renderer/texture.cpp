#include "texture.h"

#include "rendering_device.h"

Texture::Texture(ImageResourceFile* imageFile)
    : m_ImageFile(imageFile)
{
	m_TextureView = RenderingDevice::GetSingleton()->createTexture(imageFile);
	
	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	m_TextureView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&m_Texture);

	CD3D11_TEXTURE2D_DESC textureDesc;
	m_Texture->GetDesc(&textureDesc);

	m_Width = textureDesc.Width;
	m_Height = textureDesc.Height;
	m_MipLevels = textureDesc.MipLevels;
}

Texture::~Texture()
{
}
