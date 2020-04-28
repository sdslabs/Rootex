#include "material.h"

#include "shader_library.h"

Material::Material()
    : m_Shader(ShaderLibrary::GetDefaultShader())
{
}

void Material::bind()
{
	m_Shader->bind();
}

Material::Material(Shader* shader)
    : m_Shader(shader)
{
}
