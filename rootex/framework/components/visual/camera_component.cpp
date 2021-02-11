#include "camera_component.h"
#include "systems/render_system.h"

void to_json(JSON::json& j, const PostProcessingDetails& p)
{
	j["isPostProcessing"] = p.isPostProcessing;
	j["isASSAO"] = p.isASSAO;
	j["isBloom"] = p.isBloom;
	j["isSepia"] = p.isSepia;
	j["isMonochrome"] = p.isMonochrome;
	j["isGaussianBlur"] = p.isGaussianBlur;
	j["isToneMap"] = p.isToneMap;
	j["isFXAA"] = p.isFXAA;
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
	j["assaoAdaptiveQualityLimit"] = p.assaoAdaptiveQualityLimit;
	j["assaoBlurPassCount"] = p.assaoBlurPassCount;
	j["assaoDetailShadowStrength"] = p.assaoDetailShadowStrength;
	j["assaoFadeOutFrom"] = p.assaoFadeOutFrom;
	j["assaoFadeOutTo"] = p.assaoFadeOutTo;
	j["assaoHorizonAngleThreshold"] = p.assaoHorizonAngleThreshold;
	j["assaoQualityLevel"] = p.assaoQualityLevel;
	j["assaoRadius"] = p.assaoRadius;
	j["assaoShadowClamp"] = p.assaoShadowClamp;
	j["assaoShadowMultiplier"] = p.assaoShadowMultiplier;
	j["assaoShadowPower"] = p.assaoShadowPower;
	j["assaoSharpness"] = p.assaoSharpness;
}

void from_json(const JSON::json& j, PostProcessingDetails& p)
{
	p.isPostProcessing = j.at("isPostProcessing");
	p.isASSAO = j.value("isASSAO", false);
	p.isBloom = j.at("isBloom");
	p.isSepia = j.at("isSepia");
	p.isMonochrome = j.at("isMonochrome");
	p.isGaussianBlur = j.at("isGaussianBlur");
	p.isToneMap = j.at("isToneMap");
	p.isFXAA = j.value("isFXAA", false);
	p.assaoAdaptiveQualityLimit = j.value("assaoAdaptiveQualityLimit", 0.45f);
	p.assaoBlurPassCount = j.value("assaoBlurPassCount", 2);
	p.assaoDetailShadowStrength = j.value("assaoDetailShadowStrength", 0.5f);
	p.assaoFadeOutFrom = j.value("assaoFadeOutFrom", 50.0f);
	p.assaoFadeOutTo = j.value("assaoFadeOutTo", 300.0f);
	p.assaoHorizonAngleThreshold = j.value("assaoHorizonAngleThreshold", 0.06f);
	p.assaoQualityLevel = j.value("assaoQualityLevel", 2);
	p.assaoRadius = j.value("assaoRadius", 1.2f);
	p.assaoShadowClamp = j.value("assaoShadowClamp", 0.98f);
	p.assaoShadowMultiplier = j.value("assaoShadowMultiplier", 1.0f);
	p.assaoShadowPower = j.value("assaoShadowPower", 1.5f);
	p.assaoSharpness = j.value("assaoSharpness", 0.98f);
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

Matrix& CameraComponent::getViewMatrix()
{
	refreshViewMatrix();
	return m_ViewMatrix;
}

Matrix& CameraComponent::getProjectionMatrix()
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
	if (m_PostProcessingDetails.isPostProcessing)
	{
		ImGui::Checkbox("Adaptive SSAO", &m_PostProcessingDetails.isASSAO);
		if (m_PostProcessingDetails.isASSAO)
		{
			if (ImGui::TreeNodeEx("Adaptive SSAO Settings"))
			{
				ImGui::SliderInt("Quality Level", &m_PostProcessingDetails.assaoQualityLevel, -1, 3);
				ImGui::SameLine();
				ImGui::Text("Adaptive is 3");

				ImGui::SliderFloat("Adaptive Quality Limit", &m_PostProcessingDetails.assaoAdaptiveQualityLimit, 0.0f, 1.0f);
				ImGui::SliderInt("Blur Pass Count", &m_PostProcessingDetails.assaoBlurPassCount, 0, 6);
				ImGui::DragFloat("Detail Shadow", &m_PostProcessingDetails.assaoDetailShadowStrength, 0.01f, 0.0f, 5.0f);
				ImGui::DragFloat("Fade Out From", &m_PostProcessingDetails.assaoFadeOutFrom, 1.0f, 0.0f, 10000.0f);
				ImGui::DragFloat("Fade Out To", &m_PostProcessingDetails.assaoFadeOutTo, 1.0f, 0.0f, 10000.0f);
				ImGui::DragFloat("Horizon Angle Theshold", &m_PostProcessingDetails.assaoHorizonAngleThreshold, 0.01f, 0.0f, 0.2f);
				ImGui::DragFloat("Radius", &m_PostProcessingDetails.assaoRadius, 0.1f, 0.0f, 10.0f);
				ImGui::DragFloat("Shadow Clamp", &m_PostProcessingDetails.assaoShadowClamp, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Shadow Multiplier", &m_PostProcessingDetails.assaoShadowMultiplier, 0.01f, 0.0f, 5.0f);
				ImGui::DragFloat("Shadow Power", &m_PostProcessingDetails.assaoShadowPower, 0.01f, 0.5f, 5.0f);
				ImGui::DragFloat("Sharpness", &m_PostProcessingDetails.assaoSharpness, 0.01f, 0.0f, 1.0f);

				ImGui::TreePop();
			}
		}
		ImGui::Checkbox("Gaussian Blur", &m_PostProcessingDetails.isGaussianBlur);
		if (m_PostProcessingDetails.isGaussianBlur)
		{
			if (ImGui::TreeNodeEx("Gaussian Blue Settings"))
			{
				ImGui::DragFloat("Gaussian Multiplier", &m_PostProcessingDetails.gaussianBlurMultiplier, 0.01f, 0.0f, 10.0f);
				
				ImGui::TreePop();
			}
		}

		ImGui::Checkbox("Monochrome", &m_PostProcessingDetails.isMonochrome);

		ImGui::Checkbox("Sepia", &m_PostProcessingDetails.isSepia);

		ImGui::Checkbox("Bloom", &m_PostProcessingDetails.isBloom);
		if (m_PostProcessingDetails.isBloom)
		{
			if (ImGui::TreeNodeEx("Bloom Settings"))
			{
				ImGui::DragFloat("Bloom Threshold", &m_PostProcessingDetails.bloomThreshold, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Bloom Size", &m_PostProcessingDetails.bloomSize, 0.01f, 0.0f, 100.0f);
				ImGui::DragFloat("Bloom Brightness", &m_PostProcessingDetails.bloomBrightness, 0.01f, 0.0f, 5.0f);
				ImGui::DragFloat("Bloom Value", &m_PostProcessingDetails.bloomValue, 0.01f, 0.0f, 5.0f);
				ImGui::DragFloat("Bloom Base", &m_PostProcessingDetails.bloomBase, 0.01f, 0.0f, 5.0f);
				ImGui::DragFloat("Bloom Saturation", &m_PostProcessingDetails.bloomSaturation, 0.01f, 0.0f, 5.0f);
				ImGui::DragFloat("Bloom Base Saturation", &m_PostProcessingDetails.bloomBaseSaturation, 0.01f, 0.0f, 5.0f);
				
				ImGui::TreePop();
			}
		}

		ImGui::Checkbox("Tone Map", &m_PostProcessingDetails.isToneMap);
		if (m_PostProcessingDetails.isToneMap)
		{
			if (ImGui::TreeNodeEx("Tone Map Settings"))
			{
				ImGui::DragFloat("Tone Map Exposure", &m_PostProcessingDetails.toneMapExposure, 0.01f, 0.0f, 5.0f);
				ImGui::Combo("Tone Map Operator", &m_PostProcessingDetails.toneMapOperator, "None\0Saturate\0Reinhard\0ACES Filmic");
				ImGui::Combo("Tone Map Transfer Function", &m_PostProcessingDetails.toneMapTransferFunction, "Linear\0sRGB\0ST2084");
				ImGui::DragFloat("Tone Map White Nits", &m_PostProcessingDetails.toneMapWhiteNits);
				
				ImGui::TreePop();
			}
		}

		ImGui::Checkbox("FXAA", &m_PostProcessingDetails.isFXAA);
	}
}
