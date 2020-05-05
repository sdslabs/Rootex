#include "material.h"

#include "shader_library.h"

void Material::bind()
{
	m_Shader->bind();
}

JSON::json Material::getJSON() const
{
	JSON::json j;
	j["type"] = m_TypeName;
	return j;
}

Material::Material(Shader* shader, const String& typeName)
    : m_Shader(shader)
    , m_TypeName(typeName)
{
}
