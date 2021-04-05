#include "instancing_basic_material_resource_file.h"

#include "resource_loader.h"
#include "framework/systems/render_system.h"

void InstancingBasicMaterialResourceFile::Load()
{
	BufferFormat instanceBufferFormat;
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWX", D3D11_INPUT_PER_INSTANCE_DATA, 1, true, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWY", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWZ", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_ROWW", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWX", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWY", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWZ", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_INVERSE_TRANSPOSE_ROWW", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	instanceBufferFormat.push(VertexBufferElement::Type::FloatFloatFloatFloat, "INSTANCE_COLOR", D3D11_INPUT_PER_INSTANCE_DATA, 1, false, 1);
	s_Shader.reset(new Shader("rootex/core/renderer/shaders/instancing_basic_vertex_shader.hlsl", "rootex/core/renderer/shaders/basic_pixel_shader.hlsl", instanceBufferFormat));

	s_Sampler = RenderingDevice::GetSingleton()->createSS(RenderingDevice::SamplerState::Default);
}

void InstancingBasicMaterialResourceFile::Destroy()
{
	s_Shader.reset();
	s_Sampler.Reset();
}

InstancingBasicMaterialResourceFile::InstancingBasicMaterialResourceFile(const FilePath& path)
    : BasicMaterialResourceFile(Type::InstancingBasicMaterial, path)
{
	reimport();
}

void InstancingBasicMaterialResourceFile::resizeBuffer(size_t instanceCount)
{
	Vector<InstanceData> data;
	data.resize(instanceCount);
	m_VSCB = RenderingDevice::GetSingleton()->createBuffer((const char*)data.data(), sizeof(InstanceData) * data.size(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void InstancingBasicMaterialResourceFile::uploadInstanceBuffer(const Vector<InstanceData>& instanceBuffer)
{
	RenderingDevice::GetSingleton()->editBuffer((const char*)instanceBuffer.data(), sizeof(InstanceData) * instanceBuffer.size(), m_VSCB.Get());
}

void InstancingBasicMaterialResourceFile::bindVSCB()
{
	RenderingDevice::GetSingleton()->setVSCB(PER_OBJECT_VS_CPP, 1, m_VSCB.GetAddressOf());
}

void InstancingBasicMaterialResourceFile::reimport()
{
	BasicMaterialResourceFile::reimport();
	resizeBuffer(1);
}
