#include "light_system.h"
#include "core\renderer\shaders\register_locations_pixel_shader.h"

LightSystem* LightSystem::GetSingleton()
{
	static LightSystem singleton;
	return &singleton;
}

LightsInfo LightSystem::getLights()
{
	const Vector<Component*>& pointLightComponents = s_Components[PointLightComponent::s_ID];

	if (pointLightComponents.size() > MAX_POINT_LIGHTS)
	{
		WARN("Point Lights in the level are greater than " + std::to_string(MAX_POINT_LIGHTS));
	}

	LightsInfo lights;
	
	lights.cameraPos = RenderSystem::GetSingleton()->getCamera()->getPosition();

	int i = 0;
	for (; i < pointLightComponents.size() && i < MAX_POINT_LIGHTS; i++)
	{
		PointLightComponent* light = dynamic_cast<PointLightComponent*>(pointLightComponents[i]);
		TransformComponent* transform = light->getOwner()->getComponent<TransformComponent>().get();
		Vector3 transformedPosition = transform->getAbsoluteTransform().Translation();
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

	if (spotLightComponents.size() > MAX_SPOT_LIGHTS)
	{
		WARN("Spot Lights in the level are greater than " + std::to_string(MAX_SPOT_LIGHTS));
	}
	
	i = 0;
	for (; i < spotLightComponents.size() && i < MAX_SPOT_LIGHTS; i++)
	{
		SpotLightComponent* light = dynamic_cast<SpotLightComponent*>(spotLightComponents[i]);
		Matrix transform = light->getOwner()->getComponent<TransformComponent>()->getAbsoluteTransform();
		lights.spotLightInfos[i] = {
			light->m_AmbientColor, light->m_DiffuseColor, light->m_DiffuseIntensity,
			light->m_AttConst, light->m_AttLin, light->m_AttQuad,
			transform.Translation(), light->m_Range, transform.Forward(), light->m_Spot,
			light->m_AngleRange
		};
	}
	lights.spotLightCount = i;

	return lights;
}
