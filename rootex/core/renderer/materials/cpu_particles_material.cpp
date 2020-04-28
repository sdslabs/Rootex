#include "cpu_particles_material.h"

#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"

CPUParticlesMaterial::CPUParticlesMaterial()
    : Material(ShaderLibrary::GetCPUParticlesShader())
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void CPUParticlesMaterial::setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	Material::setPSConstantBuffer<PSSolidConstantBuffer>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Color], 3u);
}

void CPUParticlesMaterial::setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void CPUParticlesMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSSolidConstantBuffer({ RenderSystem::GetSingleton()->getTopMatrix() }));
}
