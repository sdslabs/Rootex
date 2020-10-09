#include "text_viewer.h"

#include "editor/editor_system.h"

#include "resource_loader.h"
#include "resource_files/text_resource_file.h"

#include "imgui.h"

void TextViewer::drawFileInfo()
{
}

ResourceFile* TextViewer::load(const FilePath& filePath)
{
	m_TextResourceFile = ResourceLoader::CreateTextResourceFile(filePath.string());
	return m_TextResourceFile;
}

void TextViewer::unload()
{
}

void TextViewer::draw(float deltaMilliseconds)
{
	drawFileInfo();
	if (m_TextResourceFile->isDirty())
	{
		ImGui::TextColored(EditorSystem::GetSingleton()->getColors().warning, "File may be changed on disk");
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			m_TextResourceFile->reimport();
		}
	}
	ImGui::Separator();
	ImGui::TextUnformatted(m_TextResourceFile->getString().c_str());
}
