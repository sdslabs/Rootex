#include "material.h"

#include "shader_library.h"
#include "texture.h"

#include "framework/systems/light_system.h"

Material::Material()
    : m_Shader(ShaderLibrary::GetDefaultShader())
{
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

Material::Material(Shader* shader)
    : m_Shader(shader)
{
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

TexturedMaterial::TexturedMaterial(Ref<Texture> diffuseTexture)
    : Material(ShaderLibrary::GetDiffuseShader())
    , m_DiffuseTexture(diffuseTexture)
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSamplerState();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
}

void ColorMaterial::bind()
{
	m_Shader->bind();
}

void TexturedMaterial::bind()
{
	m_Shader->bind();
	m_DiffuseShader->set(m_DiffuseTexture.get());
	const PSDiffuseConstantBuffer Cb = { LightSystem::GetSingleton()->getLights(), { 0.6f, 30.0f, { 0.0f, 0.0f } } };
	setPixelShaderConstantBuffer(Cb);
}

CPUParticlesMaterial::CPUParticlesMaterial()
    : Material(ShaderLibrary::GetCPUParticlesShader())
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
}

void CPUParticlesMaterial::setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	if (m_PSConstantBuffer[(int)PixelConstantBufferType::Color] == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(constantBuffer);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer;

		m_PSConstantBuffer[(int)PixelConstantBufferType::Color] = RenderingDevice::GetSingleton()->createPSConstantBuffer(&cbd, &csd, 0);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->getBufferMappedContext(m_PSConstantBuffer[(int)PixelConstantBufferType::Color].Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(m_PSConstantBuffer[(int)PixelConstantBufferType::Color].Get());
	}
}

void CPUParticlesMaterial::bind()
{
	m_Shader->bind();
}

ColorMaterial::ColorMaterial()
    : Material()
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
}

void Material::setVertexShaderConstantBuffer(const VertexConstantBufferType type, const Matrix& constantBuffer)
{
	if (m_VSConstantBuffer[(int)type] == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(constantBuffer);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer.Transpose();

		switch (type)
		{
		case VertexConstantBufferType::Model:
			m_VSConstantBuffer[(int)VertexConstantBufferType::Model] = RenderingDevice::GetSingleton()->createVSModelConstantBuffer(&cbd, &csd);
			break;
		case VertexConstantBufferType::ModelInverse:
			m_VSConstantBuffer[(int)VertexConstantBufferType::ModelInverse] = RenderingDevice::GetSingleton()->createVSModelInverseConstantBuffer(&cbd, &csd);
			break;
		default:
			break;
		}
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource = { 0 };
		RenderingDevice::GetSingleton()->getBufferMappedContext(m_VSConstantBuffer[(int)type].Get(), subresource);
		memcpy(subresource.pData, &constantBuffer.Transpose(), sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(m_VSConstantBuffer[(int)type].Get());
	}
}

void TexturedMaterial::setPixelShaderConstantBuffer(const PSDiffuseConstantBuffer& constantBuffer)
{
	if (m_PSConstantBuffer[(int)PixelConstantBufferType::Lights] == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(constantBuffer.lights);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer.lights;

		m_PSConstantBuffer[(int)PixelConstantBufferType::Lights] = RenderingDevice::GetSingleton()->createPSConstantBuffer(&cbd, &csd, 0);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->getBufferMappedContext(m_PSConstantBuffer[(int)PixelConstantBufferType::Lights].Get(), subresource);
		memcpy(subresource.pData, &constantBuffer.lights, sizeof(constantBuffer.lights));
		RenderingDevice::GetSingleton()->unmapBuffer(m_PSConstantBuffer[(int)PixelConstantBufferType::Lights].Get());
	}

	if (m_PSConstantBuffer[(int)PixelConstantBufferType::Material] == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(constantBuffer.material);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer.material;
		
		m_PSConstantBuffer[(int)PixelConstantBufferType::Material] = RenderingDevice::GetSingleton()->createPSConstantBuffer(&cbd, &csd, 1);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->getBufferMappedContext(m_PSConstantBuffer[(int)PixelConstantBufferType::Material].Get(), subresource);
		memcpy(subresource.pData, &constantBuffer.material, sizeof(constantBuffer.material));
		RenderingDevice::GetSingleton()->unmapBuffer(m_PSConstantBuffer[(int)PixelConstantBufferType::Material].Get());
	}
}

void ColorMaterial::setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	if (m_PSConstantBuffer[(int)PixelConstantBufferType::Color] == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(constantBuffer);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer;

		m_PSConstantBuffer[(int)PixelConstantBufferType::Color] = RenderingDevice::GetSingleton()->createPSConstantBuffer(&cbd, &csd, 0);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->getBufferMappedContext(m_PSConstantBuffer[(int)PixelConstantBufferType::Color].Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(m_PSConstantBuffer[(int)PixelConstantBufferType::Color].Get());
	}
}
