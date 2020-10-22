#include "camera_component.h"
#include "systems/render_system.h"

void to_json(JSON::json& j, const PostProcessingDetails& p)
{
	j["isPostProcessing"] = p.isPostProcessing;
	j["isBloom"] = p.isBloom;
	j["isSepia"] = p.isSepia;
	j["isMonochrome"] = p.isMonochrome;
	j["isGaussianBlur"] = p.isGaussianBlur;
	j["isToneMap"] = p.isToneMap;
	j["bloomThreshold"] = p.bloomThreshold;
	j["bloomSize"] = p.bloomSize;
	j["bloomBrightness"] = p.bloomBrightness;
	j["bloomValue"] = p.bloomValue;
	j["bloomBase"] = p.bloomBase;
	j["bloomSaturation"] = p.bloomSaturation;
	j["bloomBaseSaturation"] = p.bloomBaseSaturation;
	j["gaussianBlurMultiplier"] = p.gaussianBlurMultiplier;
	j["toneMapExposure"] = p.toneMapExposure;
	j["toneMapOperator"] = p.toneMapOperator;
	j["toneMapTransferFunction"] = p.toneMapTransferFunction;
	j["toneMapWhiteNits"] = p.toneMapWhiteNits;
}

void from_json(const JSON::json& j, PostProcessingDetails& p)
{
	p.isPostProcessing = j.at("isPostProcessing");
	p.isBloom = j.at("isBloom");
	p.isSepia = j.at("isSepia");
	p.isMonochrome = j.at("isMonochrome");
	p.isGaussianBlur = j.at("isGaussianBlur");
	p.isToneMap = j.at("isToneMap");
	p.bloomThreshold = j.at("bloomThreshold");
	p.bloomSize = j.at("bloomSize");
	p.bloomBrightness = j.at("bloomBrightness");
	p.bloomValue = j.at("bloomValue");
	p.bloomBase = j.at("bloomBase");
	p.bloomSaturation = j.at("bloomSaturation");
	p.bloomBaseSaturation = j.at("bloomBaseSaturation");
	p.gaussianBlurMultiplier = j.at("gaussianBlurMultiplier");
	p.toneMapExposure = j.at("toneMapExposure");
	p.toneMapOperator = j.at("toneMapOperator");
	p.toneMapTransferFunction = j.at("toneMapTransferFunction");
	p.toneMapWhiteNits = j.at("toneMapWhiteNits");
}

Component* CameraComponent::Create(const JSON::json& componentData)
{
	CameraComponent* cameraVisualComponent = new CameraComponent(
	    componentData.value("aspectRatio", Vector2{ 16.0f, 9.0f }),
	    componentData.value("fov", DirectX::XM_PI / 4.0f),
		componentData.value("near", 0.1f),
		componentData.value("far", 100.0f),
		componentData.value("postProcessingDetails", PostProcessingDetails()));
	return cameraVisualComponent;
}

CameraComponent::CameraComponent(const Vector2& aspectRatio, float fov, float nearPlane, float farPlane, const PostProcessingDetails& postProcesing)
    : m_Active(false)
    , m_FoV(fov)
    , m_AspectRatio(aspectRatio)
    , m_Near(nearPlane)
    , m_Far(farPlane)
    , m_PostProcessingDetails(postProcesing)
    , m_DependencyOnTransformComponent(this)
{
}

void CameraComponent::refreshProjectionMatrix()
{
	m_ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(m_FoV, m_AspectRatio.x / m_AspectRatio.y, m_Near, m_Far);
}

void CameraComponent::refreshViewMatrix()
{
	const Matrix& absoluteTransform = m_TransformComponent->getAbsoluteTransform();
	m_ViewMatrix = Matrix::CreateLookAt(
	    absoluteTransform.Translation(),
	    absoluteTransform.Translation() + absoluteTransform.Forward(),
	    absoluteTransform.Up());
}

void CameraComponent::onRemove()
{
	if (RenderSystem::GetSingleton()->getCamera() == this)
	{
		RenderSystem::GetSingleton()->restoreCamera();
	}
}

bool CameraComponent::setupData()
{
	refreshProjectionMatrix();
	refreshViewMatrix();
	return true;
}

const Matrix& CameraComponent::getViewMatrix()
{
	refreshViewMatrix();
	return m_ViewMatrix;
}

const Matrix& CameraComponent::getProjectionMatrix()
{
	refreshProjectionMatrix();
	return m_ProjectionMatrix;
}

JSON::json CameraComponent::getJSON() const
{
	JSON::json j;

	j["aspectRatio"] = m_AspectRatio;
	j["fov"] = m_FoV;
	j["near"] = m_Near;
	j["far"] = m_Far;
	j["postProcessingDetails"] = m_PostProcessingDetails;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void CameraComponent::draw()
{
	if (ImGui::DragFloat2("##Aspect", &m_AspectRatio.x, 0.01f, 0.1f, 100.0f))
	{
		refreshProjectionMatrix();
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();
	}
	ImGui::SameLine();
	if (ImGui::Button("Aspect Ratio"))
	{
		m_AspectRatio = { 16.0f, 9.0f };
		refreshProjectionMatrix();
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();
	}

	if (ImGui::SliderAngle("Field of View", &m_FoV, 1.0f, 180.0f))
	{
		refreshProjectionMatrix();
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();
	}

	if (ImGui::DragFloatRange2("Range", &m_Near, &m_Far, 0.01f, 0.1f, 1000.0f))
	{
		refreshProjectionMatrix();
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();
	}

	ImGui::Checkbox("Post Processing", &m_PostProcessingDetails.isPostProcessing);
	ImGui::Checkbox("Gaussian Blur", &m_PostProcessingDetails.isGaussianBlur);
	ImGui::DragFloat("Gaussian Multiplier", &m_PostProcessingDetails.gaussianBlurMultiplier, 0.01f, 0.0f, 10.0f);

	ImGui::Checkbox("Monochrome", &m_PostProcessingDetails.isMonochrome);

	ImGui::Checkbox("Sepia", &m_PostProcessingDetails.isSepia);

	ImGui::Checkbox("Bloom", &m_PostProcessingDetails.isBloom);
	ImGui::DragFloat("Bloom Threshold", &m_PostProcessingDetails.bloomThreshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Bloom Size", &m_PostProcessingDetails.bloomSize, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Bloom Brightness", &m_PostProcessingDetails.bloomBrightness, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Bloom Value", &m_PostProcessingDetails.bloomValue, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Bloom Base", &m_PostProcessingDetails.bloomBase, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Bloom Saturation", &m_PostProcessingDetails.bloomSaturation, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Bloom Base Saturation", &m_PostProcessingDetails.bloomBaseSaturation, 0.01f, 0.0f, 5.0f);

	ImGui::Checkbox("Tone Map", &m_PostProcessingDetails.isToneMap);
	ImGui::DragFloat("Tone Map Exposure", &m_PostProcessingDetails.toneMapExposure, 0.01f, 0.0f, 5.0f);
	ImGui::Combo("Tone Map Operator", &m_PostProcessingDetails.toneMapOperator, "None\0Saturate\0Reinhard\0ACES Filmic");
	ImGui::Combo("Tone Map Transfer Function", &m_PostProcessingDetails.toneMapTransferFunction, "Linear\0sRGB\0ST2084");
	ImGui::DragFloat("Tone Map White Nits", &m_PostProcessingDetails.toneMapWhiteNits);
}
#endif // ROOTEX_EDITOR
