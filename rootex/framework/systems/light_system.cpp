#include "light_system.h"

LightSystem* LightSystem::GetSingleton()
{
	static LightSystem singleton;
	return &singleton;
}

Lights LightSystem::getLights()
{
	const Vector<Component*>& pointLightComponents = s_Components[PointLightComponent::s_ID];

	if (pointLightComponents.size() > 4)
	{
		WARN("Point Lights specified are greater than 4");
	}

	Lights lights;
	
	int i = 0;
	for (; i < pointLightComponents.size() && i < 4; i++)
	{
		PointLightComponent* light = dynamic_cast<PointLightComponent*>(pointLightComponents[i]);
		TransformComponent* transform = light->getOwner()->getComponent<TransformComponent>().get();
		Vector3 transformedPosition = Vector3::Transform(transform->getPosition(), transform->getAbsoluteTransform());
		lights.pointLightInfos[i] = {
			light->m_AmbientColor, light->m_DiffuseColor, light->m_DiffuseIntensity,
			light->m_AttConst, light->m_AttLin, light->m_AttQuad,
			transformedPosition, light->m_Range
		};
	}
	lights.pointLightCount = i;

	const Vector<Component*>& directionalLightComponents = s_Components[DirectionalLightComponent::s_ID];

	if (directionalLightComponents.size() > 1)
	{
		WARN("Directional Lights specified are greater than 1");
	}
	if (directionalLightComponents.size() > 0)
	{
		DirectionalLightComponent* light = dynamic_cast<DirectionalLightComponent*>(directionalLightComponents[0]);

		lights.directionalLightInfo = {
			light->m_Direction, light->m_DiffuseIntensity, light->m_DiffuseColor,
			light->m_AmbientColor
		};
		lights.directionalLightPresent = 1;
	}

	const Vector<Component*>& spotLightComponents = s_Components[SpotLightComponent::s_ID];

	i = 0;
	for (; i < spotLightComponents.size() && i < 4; i++)
	{
		SpotLightComponent* light = dynamic_cast<SpotLightComponent*>(spotLightComponents[i]);
		TransformComponent* transform = light->getOwner()->getComponent<TransformComponent>().get();
		Vector3 direction = DirectX::XMVector3Rotate(Vector3(0.0f, 0.0f, 1.0f), transform->getRotation());
		lights.spotLightInfos[i] = {
			light->m_AmbientColor, light->m_DiffuseColor, light->m_DiffuseIntensity,
			light->m_AttConst, light->m_AttLin, light->m_AttQuad,
			transform->getPosition(), light->m_Range, direction, light->m_Spot,
			light->m_AngleRange
		};
	}
	lights.spotLightCount = i;

	return lights;
}
