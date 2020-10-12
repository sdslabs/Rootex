#include "camera_component.h"
#include "systems/render_system.h"

Component* CameraComponent::Create(const JSON::json& componentData)
{
	PostProcessingDetails details;
	if (componentData.find("postProcessing") != componentData.end())
	{
		const JSON::json& postProcessingData = componentData["postProcessing"];
		if (postProcessingData.find("isPostProcessing") != postProcessingData.end())
		{
			details.isPostProcessing = postProcessingData["isPostProcessing"];
		}
		if (postProcessingData.find("bloom") != postProcessingData.end())
		{
			details.isBloom = postProcessingData["bloom"]["isBloom"];
			details.bloomThreshold = postProcessingData["bloom"]["threshold"];
			details.bloomSize = postProcessingData["bloom"]["size"];
			details.bloomBrightness = postProcessingData["bloom"]["brightness"];
			details.bloomValue = postProcessingData["bloom"]["value"];
			details.bloomBase = postProcessingData["bloom"]["base"];
			details.bloomSaturation = postProcessingData["bloom"]["saturation"];
			details.bloomBaseSaturation = postProcessingData["bloom"]["baseSaturation"];
		}
		if (postProcessingData.find("sepia") != postProcessingData.end())
		{
			details.isSepia = postProcessingData["sepia"];
		}
		if (postProcessingData.find("monochrome") != postProcessingData.end())
		{
			details.isMonochrome = postProcessingData["monochrome"];
		}
		if (postProcessingData.find("gaussianBlur") != postProcessingData.end())
		{
			details.isGaussianBlur = postProcessingData["gaussianBlur"]["isGaussianBlur"];
			details.gaussianBlurMultiplier = postProcessingData["gaussianBlur"]["multiplier"];
		}
		if (postProcessingData.find("toneMap") != postProcessingData.end())
		{
			details.isToneMap = postProcessingData["toneMap"]["isToneMap"];
			details.toneMapExposure = postProcessingData["toneMap"]["exposure"];
			details.toneMapOperator = postProcessingData["toneMap"]["operator"];
			details.toneMapTransferFunction = postProcessingData["toneMap"]["transferFunction"];
			details.toneMapWhiteNits = postProcessingData["toneMap"]["whiteNits"];
		}
	}

	CameraComponent* cameraVisualComponent = new CameraComponent(
	    { componentData["aspectRatio"]["x"], componentData["aspectRatio"]["y"] },
	    componentData["fov"],
		componentData["near"],
		componentData["far"],
		details);
	return cameraVisualComponent;
}

Component* CameraComponent::CreateDefault()
{
	PostProcessingDetails details;

	CameraComponent* cameraVisualComponent = new CameraComponent(
	    { 16.0f, 9.0f },
		DirectX::XM_PI / 4.0f,
		0.1f, 100.0f, 
		details);
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

	j["aspectRatio"]["x"] = m_AspectRatio.x;
	j["aspectRatio"]["y"] = m_AspectRatio.y;

	j["fov"] = m_FoV;
	j["near"] = m_Near;
	j["far"] = m_Far;

	j["postProcessing"]["isPostProcessing"] = m_PostProcessingDetails.isPostProcessing;
	
	j["postProcessing"]["gaussianBlur"]["isGaussianBlur"] = m_PostProcessingDetails.isGaussianBlur;
	j["postProcessing"]["gaussianBlur"]["multiplier"] = m_PostProcessingDetails.gaussianBlurMultiplier;
	
	j["postProcessing"]["monochrome"] = m_PostProcessingDetails.isMonochrome;
	
	j["postProcessing"]["sepia"] = m_PostProcessingDetails.isSepia;
	
	j["postProcessing"]["bloom"]["isBloom"] = m_PostProcessingDetails.isBloom;
	j["postProcessing"]["bloom"]["threshold"] = m_PostProcessingDetails.bloomThreshold;
	j["postProcessing"]["bloom"]["size"] = m_PostProcessingDetails.bloomSize;
	j["postProcessing"]["bloom"]["brightness"] = m_PostProcessingDetails.bloomBrightness;
	j["postProcessing"]["bloom"]["value"] = m_PostProcessingDetails.bloomValue;
	j["postProcessing"]["bloom"]["base"] = m_PostProcessingDetails.bloomBase;
	j["postProcessing"]["bloom"]["saturation"] = m_PostProcessingDetails.bloomSaturation;
	j["postProcessing"]["bloom"]["baseSaturation"] = m_PostProcessingDetails.bloomBaseSaturation;
	
	j["postProcessing"]["toneMap"]["isToneMap"] = m_PostProcessingDetails.isToneMap;
	j["postProcessing"]["toneMap"]["exposure"] = m_PostProcessingDetails.toneMapExposure;
	j["postProcessing"]["toneMap"]["operator"] = m_PostProcessingDetails.toneMapOperator;
	j["postProcessing"]["toneMap"]["transferFunction"] = m_PostProcessingDetails.toneMapTransferFunction;
	j["postProcessing"]["toneMap"]["whiteNits"] = m_PostProcessingDetails.toneMapWhiteNits;

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
