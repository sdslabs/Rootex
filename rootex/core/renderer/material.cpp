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

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void Material::draw(const String& id)
{
	ImGui::Columns(2);

	ImGui::Text("Name");
	ImGui::NextColumn();
	ImGui::Text("%s", m_FileName.c_str());
	ImGui::NextColumn();

	ImGui::Text("Type");
	ImGui::NextColumn();
	ImGui::Text("%s", m_TypeName.c_str());

	ImGui::Columns(1);
}
#endif // ROOTEX_EDITOR
