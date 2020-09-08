#include "text_ui_component.h"

#include "systems/render_ui_system.h"
#include "renderer/rendering_device.h"

Component* TextUIComponent::Create(const JSON::json& componentData)
{
	TextUIComponent* tV2DC = new TextUIComponent(
	    ResourceLoader::CreateFontResourceFile(
	        componentData["fontResource"]),
	    componentData["text"],
	    { 
			componentData["color"]["r"],
	        componentData["color"]["g"],
	        componentData["color"]["b"],
	        componentData["color"]["a"] 
		},
	    (Mode)(int)componentData["mode"],
	    {
			componentData["origin"]["x"],
	        componentData["origin"]["y"] 
		},
	    componentData["isVisible"]);
	return tV2DC;
}

Component* TextUIComponent::CreateDefault()
{
	return new TextUIComponent(
	    ResourceLoader::CreateFontResourceFile("game/assets/fonts/noto_sans_50_regular.spritefont"),
	    "Hello World!",
	    { 1.0f, 1.0f, 1.0f, 1.0f },
	    Mode::None,
	    { 0.0f, 0.0f },
	    true);
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

void TextUIComponent::RegisterAPI(sol::state& rootex)
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

	j["fontResource"] = m_FontFile->getPath().string();
	j["text"] = m_Text;

	j["color"]["r"] = m_Color.x;
	j["color"]["g"] = m_Color.y;
	j["color"]["b"] = m_Color.z;
	j["color"]["a"] = m_Color.w;

	j["origin"]["x"] = m_Origin.x;
	j["origin"]["y"] = m_Origin.y;

	j["mode"] = (int)m_Mode;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
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

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (payloadPath.extension() == ".spritefont")
			{
				setFont(ResourceLoader::CreateFontResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Cannot assign a non-spritefont file as Font");
			}
		}
		ImGui::EndDragDropTarget();
	}
}
#endif // ROOTEX_EDITOR
