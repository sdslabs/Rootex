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
	ComponentArray<StaticPointLightComponent>& staticPointLightComponents = ECSFactory::GetAllStaticPointLightComponent();

	int i = 0;
	for (auto&& staticLight : staticPointLightComponents)
	{
		Vector3 transformedPosition = staticLight.getAbsoluteTransform().Translation();
		const PointLight& pointLight = staticLight.getPointLight();

		staticLights.pointLightInfos[i].ambientColor = pointLight.ambientColor;
		staticLights.pointLightInfos[i].diffuseColor = pointLight.diffuseColor;
		staticLights.pointLightInfos[i].diffuseIntensity = pointLight.diffuseIntensity;
		staticLights.pointLightInfos[i].attConst = pointLight.attConst;
		staticLights.pointLightInfos[i].attLin = pointLight.attLin;
		staticLights.pointLightInfos[i].attQuad = pointLight.attQuad;
		staticLights.pointLightInfos[i].lightPos = transformedPosition;
		staticLights.pointLightInfos[i].range = pointLight.range;
		i++;
		if (i == MAX_STATIC_POINT_LIGHTS)
		{
			break;
		}
	}
	return staticLights;
}

LightsInfo LightSystem::getDynamicLights()
{
	LightsInfo lights;

	Vector3 cameraPos = RenderSystem::GetSingleton()->getCamera()->getAbsolutePosition();
	lights.cameraPos = cameraPos;

	static auto pointLightSortingLambda = [&cameraPos](PointLightComponent& a, PointLightComponent& b) -> bool {
		Vector3& aa = a.getTransformComponent()->getAbsoluteTransform().Translation();
		Vector3& bb = b.getTransformComponent()->getAbsoluteTransform().Translation();
		return Vector3::DistanceSquared(cameraPos, aa) < Vector3::DistanceSquared(cameraPos, bb);
	};

	ComponentArray<PointLightComponent>& pointLightComponents = ECSFactory::GetAllPointLightComponent();
	//sort(pointLightComponents.begin(), pointLightComponents.end(), pointLightSortingLambda);

	int i = 0;
	for (auto && light: pointLightComponents)
	{
		Vector3 transformedPosition = light.getAbsoluteTransform().Translation();
		const PointLight& pointLight = light.getPointLight();

		lights.pointLightInfos[i].ambientColor = pointLight.ambientColor;
		lights.pointLightInfos[i].diffuseColor = pointLight.diffuseColor;
		lights.pointLightInfos[i].diffuseIntensity = pointLight.diffuseIntensity;
		lights.pointLightInfos[i].attConst = pointLight.attConst;
		lights.pointLightInfos[i].attLin = pointLight.attLin;
		lights.pointLightInfos[i].attQuad = pointLight.attQuad;
		lights.pointLightInfos[i].lightPos = transformedPosition;
		lights.pointLightInfos[i].range = pointLight.range;
		if (i == MAX_DYNAMIC_POINT_LIGHTS)
		{
			break;
		}
	}
	lights.pointLightCount = i;

	ComponentArray<DirectionalLightComponent>& directionalLightComponents = ECSFactory::GetAllDirectionalLightComponent();
	if (directionalLightComponents.size() > 0)
	{
		if (directionalLightComponents.size() > 1)
		{
			WARN("Directional lights specified are greater than 1. Using only the first directional light found.");
		}

		DirectionalLightComponent& light = *directionalLightComponents.begin();
		const DirectionalLight& directionalLight = light.getDirectionalLight();
		const Vector3& forward = light.getDirection();

		lights.directionalLightInfo = {
			forward,
			directionalLight.diffuseIntensity,
			directionalLight.diffuseColor,
			directionalLight.ambientColor
		};
		lights.directionalLightPresent = 1;
	}

	static auto spotLightSortingLambda = [&cameraPos](SpotLightComponent& a, SpotLightComponent& b) -> bool {
		Vector3& aa = a.getTransformComponent()->getAbsoluteTransform().Translation();
		Vector3& bb = b.getTransformComponent()->getAbsoluteTransform().Translation();
		return Vector3::DistanceSquared(cameraPos, aa) < Vector3::DistanceSquared(cameraPos, bb);
	};

	ComponentArray<SpotLightComponent>& spotLightComponents = ECSFactory::GetAllSpotLightComponent();
	//sort(spotLightComponents.begin(), spotLightComponents.end(), spotLightSortingLambda);

	i = 0;
	for (auto &&light: spotLightComponents)
	{
		Matrix transform = light.getAbsoluteTransform();
		const SpotLight& spotLight = light.getSpotLight();

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
