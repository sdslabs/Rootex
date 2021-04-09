#include "fog_component.h"

FogComponent::FogComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_Near(data.value("near", 0.0f))
    , m_Far(data.value("far", 100.0f))
    , m_Color(data.value("color", (Color)ColorPresets::Green))
{
}

JSON::json FogComponent::getJSON() const
{
	JSON::json j;

	j["near"] = m_Near;
	j["far"] = m_Far;
	j["color"] = m_Color;

	return j;
}

void FogComponent::draw()
{
	ImGui::DragFloat("Near", &m_Near);
	ImGui::DragFloat("Far", &m_Far);
	ImGui::ColorEdit4("Color", &m_Color.x);
}
