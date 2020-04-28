#include "color_material.h"

#include "renderer/shader_library.h"
#include "framework/systems/render_system.h"

ColorMaterial::ColorMaterial()
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void ColorMaterial::setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	Material::setPSConstantBuffer<PSSolidConstantBuffer>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Color], 3u);
}

void ColorMaterial::setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void ColorMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSSolidConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
}
