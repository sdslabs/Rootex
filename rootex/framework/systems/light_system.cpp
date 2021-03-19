#include "light_system.h"

#include "core/renderer/shaders/register_locations_pixel_shader.h"
#include "components/visual/light/point_light_component.h"
#include "components/visual/light/static_point_light_component.h"
#include "components/visual/light/directional_light_component.h"
#include "components/visual/light/spot_light_component.h"
#include "components/space/transform_component.h"
#include "framework/systems/render_system.h"

LightSystem::LightSystem()
    : System("LightSystem", UpdateOrder::Async, false)
{
}

LightSystem* LightSystem::GetSingleton()
{
	static LightSystem singleton;
	return &singleton;
}

StaticPointLightsInfo LightSystem::getStaticPointLights()
{
	StaticPointLightsInfo staticLights;
	Vector<Component*>& staticPointLightComponents = ECSFactory::GetComponents<StaticPointLightComponent>();

	int i = 0;
	for (; i < staticPointLightComponents.size() && i < MAX_STATIC_POINT_LIGHTS; i++)
	{
		StaticPointLightComponent* staticLight = (StaticPointLightComponent*)staticPointLightComponents[i];
		Vector3 transformedPosition = staticLight->getAbsoluteTransform().Translation();
		const PointLight& pointLight = staticLight->getPointLight();

		staticLights.pointLightInfos[i].ambientColor = pointLight.ambientColor;
		staticLights.pointLightInfos[i].diffuseColor = pointLight.diffuseColor;
		staticLights.pointLightInfos[i].diffuseIntensity = pointLight.diffuseIntensity;
		staticLights.pointLightInfos[i].attConst = pointLight.attConst;
		staticLights.pointLightInfos[i].attLin = pointLight.attLin;
		staticLights.pointLightInfos[i].attQuad = pointLight.attQuad;
		staticLights.pointLightInfos[i].lightPos = transformedPosition;
		staticLights.pointLightInfos[i].range = pointLight.range;
	}
	return staticLights;
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

	Vector<Component*>& pointLightComponents = ECSFactory::GetComponents<PointLightComponent>();
	sort(pointLightComponents.begin(), pointLightComponents.end(), sortingLambda);

	int i = 0;
	for (; i < pointLightComponents.size() && i < MAX_DYNAMIC_POINT_LIGHTS; i++)
	{
		PointLightComponent* light = (PointLightComponent*)pointLightComponents[i];
		Vector3 transformedPosition = light->getAbsoluteTransform().Translation();
		const PointLight& pointLight = light->getPointLight();

		lights.pointLightInfos[i].ambientColor = pointLight.ambientColor;
		lights.pointLightInfos[i].diffuseColor = pointLight.diffuseColor;
		lights.pointLightInfos[i].diffuseIntensity = pointLight.diffuseIntensity;
		lights.pointLightInfos[i].attConst = pointLight.attConst;
		lights.pointLightInfos[i].attLin = pointLight.attLin;
		lights.pointLightInfos[i].attQuad = pointLight.attQuad;
		lights.pointLightInfos[i].lightPos = transformedPosition;
		lights.pointLightInfos[i].range = pointLight.range;
	}
	lights.pointLightCount = i;

	const Vector<Component*>& directionalLightComponents = ECSFactory::GetComponents<DirectionalLightComponent>();

	if (directionalLightComponents.size() > 1)
	{
		WARN("Directional lights specified are greater than 1. Using only the first directional light found.");
	}

	if (directionalLightComponents.size() > 0)
	{
		DirectionalLightComponent* light = (DirectionalLightComponent*)directionalLightComponents[0];
		const DirectionalLight& directionalLight = light->getDirectionalLight();
		const Vector3& forward = light->getDirection();

		lights.directionalLightInfo = {
			forward,
			directionalLight.diffuseIntensity,
			directionalLight.diffuseColor,
			directionalLight.ambientColor
		};
		lights.directionalLightPresent = 1;
	}

	Vector<Component*>& spotLightComponents = ECSFactory::GetComponents<SpotLightComponent>();
	sort(spotLightComponents.begin(), spotLightComponents.end(), sortingLambda);

	i = 0;
	for (; i < spotLightComponents.size() && i < MAX_DYNAMIC_SPOT_LIGHTS; i++)
	{
		SpotLightComponent* light = (SpotLightComponent*)spotLightComponents[i];
		Matrix transform = light->getAbsoluteTransform();
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
