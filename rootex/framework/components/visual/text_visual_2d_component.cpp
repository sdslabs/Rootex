#include "text_visual_2d_component.h"

#include "systems/render_system.h"

Component* TextVisual2DComponent::Create(const JSON::json& componentData)
{
	TextVisual2DComponent* tV2DC = new TextVisual2DComponent(ResourceLoader::CreateFontResourceFile(componentData["fontResource"], componentData["name"]), componentData["text"]);
	return tV2DC;
}

Component* TextVisual2DComponent::CreateDefault()
{
	return new TextVisual2DComponent(ResourceLoader::CreateFontResourceFile("game/assets/fonts/noto_sans_50_regular.spritefont", "Noto Sans"), "Hello World!");
}

TextVisual2DComponent::TextVisual2DComponent(FontResourceFile* font, const String& text)
    : m_Font(font)
    , m_Text(text)
{
}

TextVisual2DComponent::~TextVisual2DComponent()
{
}

void TextVisual2DComponent::render(HierarchyGraph* graph)
{
	static Vector3 position;
	static Quaternion rotation;
	static float rotationAngle;
	static Vector3 scale;

	RenderSystem::GetSingleton()->getTopUIMatrix().Decompose(scale, rotation, position);
	rotationAngle = Vector3((Vector3(0.0f, 0.0f, 1.0f) * rotation)).z;

	m_Font->getFont()->DrawString(
	    RenderingDevice::GetSingleton()->getUIBatch().get(),
	    m_Text.c_str(),
	    position,
	    ColorPresets::White,
		rotationAngle, 
		{ 0.0f, 0.0f }, 
		scale);
}

JSON::json TextVisual2DComponent::getJSON() const
{
	JSON::json j;

	j["fontResource"] = m_Font->getPath().string();
	j["name"] = m_Font->getFontName();
	j["text"] = m_Text;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void TextVisual2DComponent::draw()
{
	ImGui::InputText("Text", &m_Text);
}
#endif // ROOTEX_EDITOR
