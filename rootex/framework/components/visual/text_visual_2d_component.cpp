#include "text_visual_2d_component.h"

#include "systems/render_system.h"

Component* TextVisual2DComponent::Create(const JSON::json& componentData)
{
	TextVisual2DComponent* tV2DC = new TextVisual2DComponent(
		ResourceLoader::CreateFontResourceFile(
			componentData["fontResource"], 
			componentData["name"]), 
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
		});
	return tV2DC;
}

Component* TextVisual2DComponent::CreateDefault()
{
	return new TextVisual2DComponent(
		ResourceLoader::CreateFontResourceFile("game/assets/fonts/noto_sans_50_regular.spritefont", "Noto Sans"), 
		"Hello World!", 
		{ 1.0f, 1.0f, 1.0f, 1.0f }, 
		Mode::None, 
		{ 0.0f, 0.0f });
}

TextVisual2DComponent::TextVisual2DComponent(FontResourceFile* font, const String& text, const Color& color, const Mode& mode, const Vector2& origin)
    : m_FontFile(font)
    , m_Text(text)
    , m_Color(color)
    , m_Mode(mode)
    , m_Origin(origin)
{
}

TextVisual2DComponent::~TextVisual2DComponent()
{
}

void TextVisual2DComponent::render()
{
	static Vector3 position;
	static Quaternion rotation;
	static float rotationAngle;
	static Vector3 scale;

	RenderSystem::GetSingleton()->getTopUIMatrix().Decompose(scale, rotation, position);
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

JSON::json TextVisual2DComponent::getJSON() const
{
	JSON::json j;

	j["fontResource"] = m_FontFile->getPath().string();
	j["name"] = m_FontFile->getFontName();
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
void TextVisual2DComponent::draw()
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

	if (ImGui::BeginCombo("Font", m_FontFile->getFontName().c_str()))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Font))
		{
			if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
			{
				setFont((FontResourceFile*)file);
			}
		}
		
		static String inputPath = "Path";
		ImGui::InputText("##Path", &inputPath);
		ImGui::SameLine();
		static String inputName = "Name";
		ImGui::InputText("##Name", &inputName);
		ImGui::SameLine();
		if (ImGui::Button("Create Font"))
		{
			if (!ResourceLoader::CreateFontResourceFile(inputPath, inputName))
			{
				WARN("Could not create font");
			}
			else
			{
				inputPath = "";
				inputName = "";
			}
		}
		ImGui::EndCombo();
	}
}
#endif // ROOTEX_EDITOR
