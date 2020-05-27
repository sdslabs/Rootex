#include "ui_component.h"

#include "systems/ui_system.h"

Component* UIComponent::Create(const JSON::json& componentData)
{
	UIComponent* ui = new UIComponent(componentData["filePath"]);
	return ui;
}

Component* UIComponent::CreateDefault()
{
	return new UIComponent("rootex/assets/default.demo");
}

UIComponent::UIComponent(const String& path)
    : m_FilePath(path)
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
	UISystem::GetSingleton()->unloadDocument(m_Document);
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
	if (ImGui::InputText("Document", &m_FilePath))
	{
		setDocument(m_FilePath);
	}
}
#endif // ROOTEX_EDITOR
