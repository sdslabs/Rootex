#include "grid_material.h"

#include "renderer/shader_library.h"

GridMaterial::GridMaterial()
    : Material(ShaderLibrary::GetGridShader())
{
}

void GridMaterial::bind()
{
	Material::bind();
}
