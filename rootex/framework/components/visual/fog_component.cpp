#include "fog_component.h"

Component* FogComponent::Create(const JSON::json& componentData)
{
	return new FogComponent(
		componentData["near"], 
		componentData["far"], 
		{ 
			componentData["color"]["r"], 
			componentData["color"]["g"], 
			componentData["color"]["b"], 
			componentData["color"]["a"] 
		});
}

Component* FogComponent::CreateDefault()
{
	return new FogComponent(0.0f, 100.0f, (Color)ColorPresets::Cyan);
}

FogComponent::FogComponent(float nearDistance, float farDistance, const Color& color)
    : m_Near(nearDistance)
    , m_Far(farDistance)
    , m_Color(color)
{
}

JSON::json FogComponent::getJSON() const
{
	JSON::json j;

	j["near"] = m_Near;
	j["far"] = m_Far;
	j["color"]["r"] = m_Color.x;
	j["color"]["g"] = m_Color.y;
	j["color"]["b"] = m_Color.z;
	j["color"]["a"] = m_Color.w;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void FogComponent::draw()
{
	ImGui::DragFloat("Near", &m_Near);
	ImGui::DragFloat("Far", &m_Far);
	ImGui::ColorEdit4("Color", &m_Color.x);
}
#endif
