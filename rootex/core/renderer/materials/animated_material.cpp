#include "animated_material.h"

#include "renderer/shader_library.h"
#include "systems/render_system.h"

AnimationMaterial::AnimationMaterial()
    : Material(ShaderLibrary::GetAnimationShader(), s_MaterialName)
{
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

Material* AnimationMaterial::Create(const JSON::json& materialData)
{
	return new AnimationMaterial();
}

Material* AnimationMaterial::CreateDefault()
{
	return new AnimationMaterial();
}

void AnimationMaterial::setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void AnimationMaterial::setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSAnimationConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Animation], 4u);
}

void AnimationMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSSolidConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AnimationMaterial::draw()
{
}
#endif // ROOTEX_EDITOR
