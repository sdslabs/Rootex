#pragma once

#include "constant_buffer.h"
#include "shader.h"

class Material
{
protected:
	Shader* m_Shader;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_PSConstantBuffer;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_VSConstantBuffer;

	Material(Shader* shader);

	template <typename T>
	void setPSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot);
	template <typename T>
	void setVSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot);

public:
	Material();
	virtual ~Material() = default;
	virtual void bind();
};

template <typename T>
void Material::setPSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& bufferPointer, UINT slot)
{
	if (bufferPointer == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer;

		bufferPointer = RenderingDevice::GetSingleton()->createPSConstantBuffer(&cbd, &csd, slot);
		RenderingDevice::GetSingleton()->setPSConstantBuffer(bufferPointer.Get(), slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->mapBuffer(bufferPointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(bufferPointer.Get());

		RenderingDevice::GetSingleton()->setPSConstantBuffer(bufferPointer.Get(), slot);
	}
}

template <typename T>
void Material::setVSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& bufferPointer, UINT slot)
{
	if (bufferPointer == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer;

		bufferPointer = RenderingDevice::GetSingleton()->createVSConstantBuffer(&cbd, &csd, slot);
		RenderingDevice::GetSingleton()->setVSConstantBuffer(bufferPointer.Get(), slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->mapBuffer(bufferPointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(bufferPointer.Get());

		RenderingDevice::GetSingleton()->setVSConstantBuffer(bufferPointer.Get(), slot);
	}
}
