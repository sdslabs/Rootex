#include "text_viewer.h"

#include "editor/editor_system.h"

#include "resource_loader.h"

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

void TextViewer::draw()
{
	drawFileInfo();
	if (m_TextResourceFile->isDirty())
	{
		ImGui::TextColored(EditorSystem::GetSingleton()->getColors().m_Warning, "File may be changed on disk");
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			ResourceLoader::Reload(m_TextResourceFile);
		}
	}
	ImGui::Separator();
	ImGui::TextUnformatted(m_TextResourceFile->getString().c_str());
}
