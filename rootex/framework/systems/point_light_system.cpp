#include "point_light_system.h"

void PointLightSystem::update(float deltaMilliseconds)
{
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

Lights PointLightSystem::getPointLights()
{
	const Vector<Component*> pointLightComponents = s_Components[PointLightComponent::s_ID];

	if (pointLightComponents.size() > 4)
	{
		WARN("Point Lights used are greater than 4");
	}

	Lights lights;
	
	int i = 0;
	for (; i < pointLightComponents.size() && i<4; i++)
	{
		PointLightComponent* light = dynamic_cast<PointLightComponent*>(pointLightComponents[i]);
		TransformComponent* transform = light->getOwner()->getComponent<TransformComponent>();
		lights.lightInfos[i] = {
			light->m_ambientColor, light->m_diffuseColor, light->m_diffuseIntensity,
			light->m_constAtt, light->m_linAtt, light->m_quadAtt,
			transform->getPosition(),
			0.0f
		};
	}
	lights.lightCount = i;
	return lights;
}
