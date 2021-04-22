#include "file_editor.h"

#include "editor/editor_system.h"

#define TEXT_EDITOR_SIZE_LIMIT 100000

Variant FileEditor::editFile(const Event* e)
{
	FilePath path = Extract<String>(e->getData());
	if (path.extension() == ".lua"
	    || path.extension() == ".json"
	    || path.extension() == ".rmat"
	    || path.extension() == ".rml"
	    || path.extension() == ".rcss")
	{
		m_Editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	}
	else if (path.extension() == ".hlsl" || path.extension() == ".hlsli")
	{
		m_Editor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
	}
	else
	{
		OS::EditFileInSystemEditor(path.generic_string());
		return true;
	}

	if (OS::IsExists(path.generic_string()))
	{
		FileBuffer fileBuffer = OS::LoadFileContents(path.generic_string());
		String text(fileBuffer.begin(), fileBuffer.end());

		if (text.size() > TEXT_EDITOR_SIZE_LIMIT)
		{
			WARN("File is too big to edit: " + path.generic_string());
			return false;
		}

		m_Editor.SetText(text);
	}
	else
	{
		ERR("Could not find file for editing: " + path.generic_string());
	}

	m_EditFilePath = path.generic_string();

	ImGui::SetWindowFocus("File Editor");

	return true;
}

FileEditor::FileEditor()
{
	m_Binder.bind(EditorEvents::EditorEditFile, this, &FileEditor::editFile);
}

void FileEditor::draw(float deltaMilliseconds)
{
	TextEditor::Coordinates cpos = m_Editor.GetCursorPosition();
	ImGui::Begin("File Editor", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "", nullptr, !m_EditFilePath.empty()))
			{
				String textToSave = m_Editor.GetText();
				PANIC(!OS::SaveFile(m_EditFilePath, textToSave.c_str(), textToSave.size()), "Could not save file: " + m_EditFilePath);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = m_Editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
			{
				m_Editor.SetReadOnly(ro);
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && m_Editor.CanUndo()))
			{
				m_Editor.Undo();
			}
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && m_Editor.CanRedo()))
			{
				m_Editor.Redo();
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, m_Editor.HasSelection()))
			{
				m_Editor.Copy();
			}
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && m_Editor.HasSelection()))
			{
				m_Editor.Cut();
			}
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && m_Editor.HasSelection()))
			{
				m_Editor.Delete();
			}
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
			{
				m_Editor.Paste();
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
			{
				m_Editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(m_Editor.GetTotalLines(), 0));
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
			{
				m_Editor.SetPalette(TextEditor::GetDarkPalette());
			}
			if (ImGui::MenuItem("Light palette"))
			{
				m_Editor.SetPalette(TextEditor::GetLightPalette());
			}
			if (ImGui::MenuItem("Retro blue palette"))
			{
				m_Editor.SetPalette(TextEditor::GetRetroBluePalette());
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (!m_EditFilePath.empty())
	{
		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, m_Editor.GetTotalLines(),
		    m_Editor.IsOverwrite() ? "Ovr" : "Ins",
		    m_Editor.CanUndo() ? "*" : " ",
		    m_Editor.GetLanguageDefinition().mName.c_str(), m_EditFilePath.c_str());
	}

	EditorSystem::GetSingleton()->pushMonospaceFont();
	m_Editor.Render("TextEditor");
	EditorSystem::GetSingleton()->popFont();

	ImGui::End();
}
