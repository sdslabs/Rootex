#include "god_rays_component.h"

#include "entity.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(GodRaysComponent);

GodRaysComponent::GodRaysComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_NumSamples(data.value("numSamples", 100))
    , m_Density(data.value("density", 1.0f))
    , m_Weight(data.value("weight", 0.01f))
    , m_Decay(data.value("decay", 1.0f))
    , m_Exposure(data.value("exposure", 1.0f))
    , m_DependencyOnTransformComponent(this)
{
}

Vector3 GodRaysComponent::getScreenSpacePosition()
{
	CameraComponent* renderCamera = RenderSystem::GetSingleton()->getCamera();

	const Matrix& view = renderCamera->getViewMatrix();
	const Matrix& proj = renderCamera->getProjectionMatrix();
	const Matrix& model = getTransformComponent()->getAbsoluteTransform();

	Matrix mvp = model * view * proj;
	Vector4 dc = Vector4::Transform(Vector4(0.0f, 0.0f, 0.0f, 1.0f), mvp);
	Vector3 ndc = Vector3(dc.x, -dc.y, dc.z) / dc.w;
	Vector3 screenSpacePosition = ndc / 2.0f + Vector3(0.5f, 0.5f, 0.5f);

	return screenSpacePosition;
}

JSON::json GodRaysComponent::getJSON() const
{
	JSON::json j;

	j["numSamples"] = m_NumSamples;
	j["density"] = m_Density;
	j["weight"] = m_Weight;
	j["decay"] = m_Decay;
	j["exposure"] = m_Exposure;

	return j;
}

void GodRaysComponent::draw()
{
	int minNumSamples = 0;
	int maxNumSamples = 100;
	ImGui::DragInt("Num Samples", &m_NumSamples, 1.0f, minNumSamples, maxNumSamples);

	float minDensity = 0.0f;
	float maxDensity = 2.0f;
	ImGui::DragFloat("Density", &m_Density, 0.01f, minDensity, maxDensity);

	float minWeight = 0.0f;
	float maxWeight = 0.1f;
	ImGui::DragFloat("Weight", &m_Weight, 0.001f, minWeight, maxWeight);

	float minDecay = 0.95f;
	float maxDecay = 1.05f;
	ImGui::DragFloat("Decay", &m_Decay, 0.001f, minDecay, maxDecay);

	float minExposure = 0.0f;
	float maxExposure = 2.0f;
	ImGui::DragFloat("Exposure", &m_Exposure, 0.02f, minExposure, maxExposure);
}
