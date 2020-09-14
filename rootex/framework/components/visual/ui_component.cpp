#include "ui_component.h"

#include "systems/ui_system.h"

#include "resource_loader.h"

Component* UIComponent::Create(const JSON::json& componentData)
{
	UIComponent* ui = new UIComponent(componentData["filePath"]);
	return ui;
}

Component* UIComponent::CreateDefault()
{
	return new UIComponent("rootex/assets/rml/demo.rml");
}

UIComponent::UIComponent(const String& path)
    : m_FilePath(path)
    , m_Document(nullptr)
{
	setDocument(path);
}

UIComponent::~UIComponent()
{
	if (m_Document)
	{
		UISystem::GetSingleton()->unloadDocument(m_Document);
	}
}

void UIComponent::setDocument(const String& path)
{
	if (m_Document)
	{
		UISystem::GetSingleton()->unloadDocument(m_Document);
	}

	m_FilePath = path;
	m_Document = UISystem::GetSingleton()->loadDocument(m_FilePath);
}

JSON::json UIComponent::getJSON() const
{
	JSON::json j;

	j["filePath"] = m_FilePath;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void UIComponent::draw()
{
	ImGui::BeginGroup();
	if (ImGui::InputText("##Document", &m_FilePath, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		setDocument(m_FilePath);
	}
	ImGui::SameLine();
	if (ImGui::Button("Document"))
	{
		EventManager::GetSingleton()->call("OpenDocument", "EditorOpenFile", m_FilePath);
	}
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (IsFileSupported(payloadPath.extension().string(), ResourceFile::Type::Text))
			{
				setDocument(payloadPath.string());
			}
			else
			{
				WARN("Unsupported file format for RML");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::Button("Refresh"))
	{
		setDocument(m_FilePath);
	}
}
#endif // ROOTEX_EDITOR
