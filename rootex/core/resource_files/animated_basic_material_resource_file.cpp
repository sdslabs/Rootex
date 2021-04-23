#include "animated_basic_material_resource_file.h"

void AnimatedBasicMaterialResourceFile::Load()
{
	BufferFormat animationFormat;
	animationFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	animationFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	animationFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	animationFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	animationFormat.push(VertexBufferElement::Type::IntIntIntInt, "BONEINDICES", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	animationFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "BONEWEIGHTS", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	s_Shader.reset(new Shader("rootex/core/renderer/shaders/animation_basic_vertex_shader.hlsl", "rootex/core/renderer/shaders/basic_pixel_shader.hlsl", animationFormat));
}

void AnimatedBasicMaterialResourceFile::Destroy()
{
	s_Shader.reset();
}

AnimatedBasicMaterialResourceFile::AnimatedBasicMaterialResourceFile(const FilePath& path)
    : BasicMaterialResourceFile(Type::AnimatedBasicMaterial, path)
{
	reimport();
}

void AnimatedBasicMaterialResourceFile::uploadAnimationBuffer(const PerModelAnimationVSCBData& animationBuffer)
{
	RenderingDevice::GetSingleton()->editBuffer<PerModelAnimationVSCBData>(animationBuffer, m_BonesVSCB.Get());
}

void AnimatedBasicMaterialResourceFile::bindShader()
{
	s_Shader->bind();
}

void AnimatedBasicMaterialResourceFile::bindVSCB()
{
	BasicMaterialResourceFile::bindVSCB();
	RenderingDevice::GetSingleton()->setVSCB(BONES_VS_CPP, 1, m_BonesVSCB.GetAddressOf());
}

void AnimatedBasicMaterialResourceFile::reimport()
{
	BasicMaterialResourceFile::reimport();
	m_BonesVSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelAnimationVSCBData>(PerModelAnimationVSCBData(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}
