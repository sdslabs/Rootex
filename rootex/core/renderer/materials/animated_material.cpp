#include "animated_material.h"

#include "renderer/shader_library.h"

AnimationMaterial::AnimationMaterial()
    : Material(ShaderLibrary::GetAnimationShader(), s_MaterialName)
{
}

Material* AnimationMaterial::Create(const JSON::json& materialData)
{
	return new AnimationMaterial();
}

Material* AnimationMaterial::CreateDefault()
{
	return new AnimationMaterial();
}

void AnimationMaterial::setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSAnimationConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Animation], 4u);
}

void AnimationMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSAnimationConstantBuffer(m_BoneTransforms));
}

JSON::json AnimationMaterial::getJSON() const
{
	return JSON::json::object();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AnimationMaterial::draw()
{
}
#endif // ROOTEX_EDITOR
