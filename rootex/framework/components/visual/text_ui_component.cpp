#include "text_ui_component.h"

#include "systems/render_ui_system.h"
#include "renderer/rendering_device.h"

Component* TextUIComponent::Create(const JSON::json& componentData)
{
	TextUIComponent* tV2DC = new TextUIComponent(
	    ResourceLoader::CreateFontResourceFile(componentData.value("fontResource", "game/assets/fonts/noto_sans_50_regular.spritefont")),
	    componentData.value("text", "Hello Rootex!"),
	    componentData.value("color", (Color)ColorPresets::White),
	    (Mode)(int)componentData.value("mode", (int)Mode::None),
	    componentData.value("origin", Vector2::Zero),
	    componentData.value("isVisible", true));
	return tV2DC;
}

TextUIComponent::TextUIComponent(FontResourceFile* font, const String& text, const Color& color, const Mode& mode, const Vector2& origin, const bool& isVisible)
    : RenderUIComponent(isVisible)
	, m_FontFile(font)
    , m_Text(text)
    , m_Color(color)
    , m_Mode(mode)
    , m_Origin(origin)
{
}

void TextUIComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<TextUIComponent> textUIComponent = rootex.new_usertype<TextUIComponent>(
	    "TextUIComponent",
	    sol::base_classes, sol::bases<Component, RenderUIComponent>());
	rootex["Entity"]["getTextUI"] = &Entity::getComponent<TextUIComponent>;
	textUIComponent["setFont"] = &TextUIComponent::setFont;
	textUIComponent["setText"] = &TextUIComponent::setText;
}

void TextUIComponent::render()
{
	static Vector3 position;
	static Quaternion rotation;
	static float rotationAngle;
	static Vector3 scale;

	RenderUISystem::GetSingleton()->getTopUIMatrix().Decompose(scale, rotation, position);
	rotationAngle = Vector3((Vector3(0.0f, 0.0f, 1.0f) * rotation)).z;

	m_FontFile->getFont()->DrawString(
	    RenderingDevice::GetSingleton()->getUIBatch().get(),
	    m_Text.c_str(),
	    position,
	    m_Color,
	    rotationAngle,
	    -m_Origin,
	    scale,
	    (DirectX::SpriteEffects)m_Mode);
}

JSON::json TextUIComponent::getJSON() const
{
	JSON::json j = RenderUIComponent::getJSON();

	j["fontResource"] = m_FontFile->getPath().generic_string();
	j["text"] = m_Text;
	j["color"] = m_Color;
	j["origin"] = m_Origin;
	j["mode"] = (int)m_Mode;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
#include "utility/imgui_helpers.h"
void TextUIComponent::draw()
{
	ImGui::InputText("Text", &m_Text);
	ImGui::ColorEdit4("Color", &m_Color.x);
	ImGui::DragFloat2("Origin", &m_Origin.x);

	static const char* modes[] = {
		"None",
		"FlipX",
		"FlipY",
		"FlipXY"
	};

	static int choice = 0;
	ImGui::Combo("Mode", &choice, modes, IM_ARRAYSIZE(modes));
	m_Mode = (Mode)choice;

	ImGui::BeginGroup();
	
	static String inputPath = "Path";
	ImGui::InputText("##Path", &inputPath);
	ImGui::SameLine();
	if (ImGui::Button("Create Font"))
	{
		if (!ResourceLoader::CreateFontResourceFile(inputPath))
		{
			WARN("Could not create font");
		}
		else
		{
			inputPath = "";
		}
	}

	ImGui::EndGroup();

	if (ImGui::Button(ICON_ROOTEX_EXTERNAL_LINK "##Font"))
	{
		igfd::ImGuiFileDialog::Instance()->OpenDialog("Font", "Choose Font", SupportedFiles.at(ResourceFile::Type::Font), "game/assets/");
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog("Font"))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk)
		{
			String filePathName = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName()).generic_string();
			setFont(ResourceLoader::CreateFontResourceFile(filePathName));
		}

		igfd::ImGuiFileDialog::Instance()->CloseDialog("Font");
	}
}
#endif // ROOTEX_EDITOR
