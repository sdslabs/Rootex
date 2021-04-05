#include "material_resource_file.h"

MaterialResourceFile::MaterialResourceFile(const Type& type, const FilePath& path)
    : ResourceFile(type, path)
{
}

JSON::json MaterialResourceFile::getJSON() const
{
	JSON::json j;

	j["alpha"] = m_IsAlpha;

	return j;
}

void MaterialResourceFile::readJSON(const JSON::json& j)
{
	m_IsAlpha = j.value("alpha", false);
}

void MaterialResourceFile::draw()
{
	ImGui::Checkbox("Alpha", &m_IsAlpha);
}

bool MaterialResourceFile::saveMaterialData(const JSON::json& j)
{
	String fileString = j.dump(4);
	if (!OS::SaveFile(getPath(), fileString.c_str(), fileString.size()))
	{
		WARN("Could not save material to file: " + getPath().generic_string());
		return false;
	}
	return true;
}
