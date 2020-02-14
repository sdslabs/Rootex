#include "light_system.h"

void LightSystem::update(float deltaMilliseconds)
{
}

LightSystem* LightSystem::GetSingleton()
{
	static LightSystem singleton;
	return &singleton;
}

Lights LightSystem::getPointLights()
{
	const Vector<Component*> pointLightComponents = s_Components[PointLightComponent::s_ID];

	if (pointLightComponents.size() > 4)
	{
		WARN("Point Lights used are greater than 4");
	}

	Lights lights;
	
	int i = 0;
	for (; i < pointLightComponents.size() && i < 4; i++)
	{
		PointLightComponent* light = dynamic_cast<PointLightComponent*>(pointLightComponents[i]);
		TransformComponent* transform = light->getOwner()->getComponent<TransformComponent>();
		Vector3 transformedPosition = Vector3::Transform(transform->getPosition(), transform->getAbsoluteTransform());
		lights.lightInfos[i] = {
			light->m_ambientColor, light->m_diffuseColor, light->m_diffuseIntensity,
			light->m_constAtt, light->m_linAtt, light->m_quadAtt,
			transformedPosition, light->m_range
		};
	}
	lights.lightCount = i;
	return lights;
}
