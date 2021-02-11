#include "material.h"

#include "shader_library.h"

JSON::json Material::getJSON() const
{
	JSON::json j;
	j["type"] = m_TypeName;
	j["isAlpha"] = m_IsAlpha;
	return j;
}

Material::Material(Shader* shader, const String& typeName, bool isAlpha)
    : m_Shader(shader)
    , m_TypeName(typeName)
    , m_IsAlpha(isAlpha)
{
}

void Material::draw()
{
	ImGui::Columns(2);

	ImGui::Text("Material Name");
	ImGui::NextColumn();
	ImGui::Text("%s", m_FileName.c_str());
	ImGui::NextColumn();

	ImGui::Text("Material Type");
	ImGui::NextColumn();
	ImGui::Text("%s", m_TypeName.c_str());
	ImGui::NextColumn();

	ImGui::Text("Alpha");
	ImGui::NextColumn();
	ImGui::Checkbox("##Alpha", &m_IsAlpha);

	ImGui::Columns(1);
}
