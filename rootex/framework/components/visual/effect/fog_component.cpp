#include "fog_component.h"

Ptr<Component> FogComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<FogComponent>(
	    componentData.value("near", 0.0f),
	    componentData.value("far", 100.0f),
	    componentData.value("color", (Color)ColorPresets::Cyan));
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
	j["color"] = m_Color;

	return j;
}

void FogComponent::draw()
{
	ImGui::DragFloat("Near", &m_Near);
	ImGui::DragFloat("Far", &m_Far);
	ImGui::ColorEdit4("Color", &m_Color.x);
}
