#include "light_system.h"
#include "core/renderer/shaders/register_locations_pixel_shader.h"

LightSystem::LightSystem()
    : System("LightSystem", UpdateOrder::Async, false)
{
}

LightSystem* LightSystem::GetSingleton()
{
	static LightSystem singleton;
	return &singleton;
}

LightsInfo LightSystem::getStaticLights()
{
	return LightsInfo();
}

LightsInfo LightSystem::getDynamicLights()
{
	LightsInfo lights;

	Vector3 cameraPos = RenderSystem::GetSingleton()->getCamera()->getAbsolutePosition();
	lights.cameraPos = cameraPos;

	static auto sortingLambda = [&cameraPos](const Component* a, const Component* b) -> bool {
		Vector3& aa = a->getOwner()->getComponent<TransformComponent>()->getAbsoluteTransform().Translation();
		Vector3& bb = b->getOwner()->getComponent<TransformComponent>()->getAbsoluteTransform().Translation();
		return Vector3::DistanceSquared(cameraPos, aa) < Vector3::DistanceSquared(cameraPos, bb);
	};

	Vector<Component*>& pointLightComponents = s_Components[PointLightComponent::s_ID];
	sort(pointLightComponents.begin(), pointLightComponents.end(), sortingLambda);

	int i = 0;
	for (; i < pointLightComponents.size() && i < MAX_DYNAMIC_POINT_LIGHTS; i++)
	{
		PointLightComponent* light = dynamic_cast<PointLightComponent*>(pointLightComponents[i]);
		TransformComponent* transform = light->getOwner()->getComponent<TransformComponent>().get();
		Vector3 transformedPosition = transform->getAbsoluteTransform().Translation();
		const PointLight& pointLight = light->getPointLight();
		
		lights.pointLightInfos[i] = {
			pointLight.ambientColor, 
			pointLight.diffuseColor, 
			pointLight.diffuseIntensity,
			pointLight.attConst, 
			pointLight.attLin, 
			pointLight.attQuad,
			transformedPosition, 
			pointLight.range
		};
	}
	lights.pointLightCount = i;

	const Vector<Component*>& directionalLightComponents = s_Components[DirectionalLightComponent::s_ID];

	if (directionalLightComponents.size() > 1)
	{
		WARN("Directional lights specified are greater than 1. Using only the first directional light found.");
	}

	if (directionalLightComponents.size() > 0)
	{
		DirectionalLightComponent* light = dynamic_cast<DirectionalLightComponent*>(directionalLightComponents[0]);
		const DirectionalLight& directionalLight = light->getDirectionalLight();

		lights.directionalLightInfo = {
			directionalLight.direction, 
			directionalLight.diffuseIntensity, 
			directionalLight.diffuseColor,
			directionalLight.ambientColor
		};
		lights.directionalLightPresent = 1;
	}

	Vector<Component*>& spotLightComponents = s_Components[SpotLightComponent::s_ID];
	sort(spotLightComponents.begin(), spotLightComponents.end(), sortingLambda);

	i = 0;
	for (; i < spotLightComponents.size() && i < MAX_DYNAMIC_SPOT_LIGHTS; i++)
	{
		SpotLightComponent* light = dynamic_cast<SpotLightComponent*>(spotLightComponents[i]);
		Matrix transform = light->getOwner()->getComponent<TransformComponent>()->getAbsoluteTransform();
		const SpotLight& spotLight = light->getSpotLight();

		lights.spotLightInfos[i] = {
			spotLight.ambientColor,
			spotLight.diffuseColor, 
			spotLight.diffuseIntensity,
			spotLight.attConst, 
			spotLight.attLin, 
			spotLight.attQuad,
			transform.Translation(), 
			spotLight.range, 
			transform.Forward(), 
			spotLight.spot,
			cos(spotLight.angleRange)
		};
	}
	lights.spotLightCount = i;

	return lights;
}
