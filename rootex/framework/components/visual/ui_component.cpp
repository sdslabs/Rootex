#include "ui_component.h"

#include "systems/ui_system.h"

#include "resource_loader.h"

Component* UIComponent::Create(const JSON::json& componentData)
{
	UIComponent* ui = new UIComponent(componentData.value("filePath", "rootex/assets/rml/demo.rml"));
	return ui;
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
		UISystem::GetSingleton()->getContext()->Update();
	}
}

void UIComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<UIComponent> ui = rootex.new_usertype<UIComponent>(
	    "UIComponent",
	    sol::base_classes, sol::bases<Component>());
	rootex["Entity"]["getUI"] = &Entity::getComponent<UIComponent>;
	ui["getDocumentID"] = [](UIComponent* ui) { return ui->getDocument()->GetId(); };
}

void UIComponent::setDocument(const String& path)
{
	if (m_Document)
	{
		UISystem::GetSingleton()->unloadDocument(m_Document);
	}

	try
	{
		m_Document = UISystem::GetSingleton()->loadDocument(m_FilePath);
		m_FilePath = path;
	}
    catch(std::exception e)
	{
		WARN(e.what());
	}
}

JSON::json UIComponent::getJSON() const
{
	JSON::json j;

	j["filePath"] = m_FilePath;

	return j;
}

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

	if (ImGui::Button(ICON_ROOTEX_EXTERNAL_LINK "##Document"))
	{
		if (Optional<String> result = OS::SelectFile("RML Document(*.rml)\0*.rml\0", "game/assets/"))
		{
			setDocument(*result);
		}
	}

	if (ImGui::Button("Refresh"))
	{
		setDocument(m_FilePath);
	}
}
