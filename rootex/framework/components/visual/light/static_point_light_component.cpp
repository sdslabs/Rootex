#include "static_point_light_component.h"

#include "systems/render_system.h"

DEFINE_COMPONENT(StaticPointLightComponent);

StaticPointLightComponent::StaticPointLightComponent(Entity& owner, const JSON::json& data)
    : PointLightComponent(owner, data)
{
}

void StaticPointLightComponent::draw()
{
	if (ImGui::Button("Update Lights"))
	{
		RenderSystem::GetSingleton()->updateStaticLights();
	}
	PointLightComponent::draw();
}
