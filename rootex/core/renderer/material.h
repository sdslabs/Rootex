#pragma once

#include "constant_buffer.h"
#include "shader.h"

class Material
{
protected:
	Shader* m_Shader;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_PSConstantBuffer;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_VSConstantBuffer;
	String m_FileName;
	String m_TypeName;
	bool m_IsAlpha;

	Material(Shader* shader, const String& typeName, bool isAlpha);

public:
	template <typename T>
	static void SetPSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot);
	template <typename T>
	static void SetVSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot);

	Material() = delete;
	virtual ~Material() = default;

	virtual void bind() = 0;
	
	virtual ID3D11ShaderResourceView* getPreview() = 0;

	bool isAlpha() { return m_IsAlpha; }
	String getFileName() { return m_FileName; };
	String getTypeName() { return m_TypeName; };
	String getFullName() { return m_FileName + " - " + m_TypeName; };
	Shader* getShader() const { return m_Shader; }
	
	virtual JSON::json getJSON() const;
	
	void setFileName(const String& fileName) { m_FileName = fileName; };
	
	virtual void draw();
};

template <typename T>
void Material::SetPSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& bufferPointer, UINT slot)
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

		bufferPointer = RenderingDevice::GetSingleton()->createPSCB(&cbd, &csd);
		RenderingDevice::GetSingleton()->setPSCB(bufferPointer.Get(), slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->mapBuffer(bufferPointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(T));
		RenderingDevice::GetSingleton()->unmapBuffer(bufferPointer.Get());

		RenderingDevice::GetSingleton()->setPSCB(bufferPointer.Get(), slot);
	}
}

template <typename T>
void Material::SetVSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& bufferPointer, UINT slot)
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

		bufferPointer = RenderingDevice::GetSingleton()->createVSCB(&cbd, &csd);
		RenderingDevice::GetSingleton()->setVSCB(bufferPointer.Get(), slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->mapBuffer(bufferPointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(bufferPointer.Get());

		RenderingDevice::GetSingleton()->setVSCB(bufferPointer.Get(), slot);
	}
}
