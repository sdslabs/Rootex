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
}

void InstancingBasicMaterialResourceFile::Destroy()
{
	s_Shader.reset();
}

InstancingBasicMaterialResourceFile::InstancingBasicMaterialResourceFile(const FilePath& path)
    : BasicMaterialResourceFile(Type::InstancingBasicMaterial, path)
{
	reimport();
}

void InstancingBasicMaterialResourceFile::bindShader()
{
	s_Shader->bind();
}
