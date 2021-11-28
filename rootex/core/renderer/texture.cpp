#include "Texture.h"

#include "rendering_device.h"
#include "resource_loader.h"

GPUTexture::GPUTexture(const char* pixelData, int width, int height)
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

GPUTexture::GPUTexture(const char* imageFileData, size_t size)
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

unsigned char* GPUTexture::download()
{
	return RenderingDevice::GetSingleton()->downloadTexture(m_Texture.Get(), m_Width, m_Height);
}

CPUTexture::CPUTexture(unsigned char* pixelData, int width, int height)
    : m_Buffer(pixelData)
    , m_Width(width)
    , m_Height(height)
{
}

CPUTexture::~CPUTexture()
{
	delete[] m_Buffer;
}

void CPUTexture::setPixel(int x, int y, Color color)
{
	m_Buffer[x * m_Width + y] = unsigned char(color.x * 255.0f);
	m_Buffer[x * m_Width + y + 1] = unsigned char(color.y * 255.0f);
	m_Buffer[x * m_Width + y + 2] = unsigned char(color.z * 255.0f);
	m_Buffer[x * m_Width + y + 3] = unsigned char(color.w * 255.0f);
}

Color CPUTexture::getPixel(int x, int y)
{
	return Color(
	    m_Buffer[x * m_Width + y] / 255.0f,
	    m_Buffer[x * m_Width + y + 1] / 255.0f,
	    m_Buffer[x * m_Width + y + 2] / 255.0f,
	    m_Buffer[x * m_Width + y + 3] / 255.0f);
}

TextureCube::TextureCube(const char* imageDDSFileData, size_t size)
{
	m_TextureView = RenderingDevice::GetSingleton()->createDDSTexture(imageDDSFileData, size);
}
