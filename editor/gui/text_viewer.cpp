#include "text_viewer.h"

#include "resource_loader.h"

#include "imgui.h"

void TextViewer::load(const FilePath& filePath)
{
	m_TextResourceFile = ResourceLoader::CreateTextResourceFile(filePath.string());
}

void TextViewer::unload()
{
}

void TextViewer::draw()
{
	ImGui::TextWrapped(m_TextResourceFile->getString().c_str());
}
