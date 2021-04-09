#include "text_ui_component.h"

#include "systems/render_ui_system.h"
#include "renderer/rendering_device.h"

TextUIComponent::TextUIComponent(Entity& owner, const JSON::json& data)
    : RenderUIComponent(owner, data)
    , m_FontFile(ResourceLoader::CreateFontResourceFile(data.value("fontResource", "rootex/assets/fonts/lato_30_regular.spritefont")))
    , m_Text(data.value("text", "Hello Rootex!"))
    , m_Color(data.value("color", (Color)ColorPresets::White))
    , m_Mode((Mode)(int)data.value("mode", (int)Mode::None))
    , m_Origin(data.value("origin", Vector2::Zero))
    , m_Rotation(data.value("rotation", 0.0f))
{
}

void TextUIComponent::render()
{
	static Vector3 position;
	static Quaternion rotation;
	static Vector3 scale;

	RenderUISystem::GetSingleton()->getTopUIMatrix().Decompose(scale, rotation, position);

	m_FontFile->getFont()->DrawString(
	    RenderingDevice::GetSingleton()->getUIBatch().get(),
	    m_Text.c_str(),
	    position,
	    m_Color,
	    m_Rotation,
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
	j["rotation"] = m_Rotation;
	j["mode"] = (int)m_Mode;

	return j;
}

void TextUIComponent::draw()
{
	ImGui::InputText("Text", &m_Text);
	ImGui::ColorEdit4("Color", &m_Color.x);
	ImGui::DragFloat2("Origin", &m_Origin.x);
	ImGui::DragFloat("Rotation", &m_Rotation);

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
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Font), "game/assets/"))
		{
			setFont(ResourceLoader::CreateFontResourceFile(*result));
		}
	}
}
