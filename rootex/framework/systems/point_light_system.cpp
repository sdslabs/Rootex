#include "point_light_system.h"

#include "components/visual/point_light_component.h"

void PointLightSystem::update(float deltaMilliseconds)
{
	const Vector<Component*> testComponents = s_Components[PointLightComponent::s_ID];

	for (auto& testComponent : testComponents)
	{
		OS::PrintWarning("TestComponent was processed by TestSystem");
	}
}

void PointLightSystem::apply()
{
	const Vector<Component*> pointLightComponents = s_Components[PointLightComponent::s_ID];

	for (auto& pointLightComponent : pointLightComponents)
	{
		PointLightComponent* light = (PointLightComponent *)(pointLightComponent);
	}
}

PointLightSystem* PointLightSystem::GetSingleton()
{
	static PointLightSystem singleton;
	return &singleton;
}
