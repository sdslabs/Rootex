#include "camera_component.h"
#include "systems/render_system.h"

Component* CameraComponent::Create(const JSON::json& componentData)
{
	CameraComponent* cameraVisualComponent = new CameraComponent(
	    { componentData["aspectRatio"]["x"], componentData["aspectRatio"]["y"] },
	    componentData["fov"],
		componentData["near"],
		componentData["far"]);
	return cameraVisualComponent;
}

Component* CameraComponent::CreateDefault()
{
	CameraComponent* cameraVisualComponent = new CameraComponent(
	    { 16.0f, 9.0f },
		DirectX::XM_PI / 4.0f,
		0.1f, 100.0f);
	return cameraVisualComponent;
}

CameraComponent::CameraComponent(const Vector2& aspectRatio, float fov, float nearPlane, float farPlane)
    : m_Active(false)
    , m_FoV(fov)
    , m_AspectRatio(aspectRatio)
    , m_Near(nearPlane)
    , m_Far(farPlane)
    , m_TransformComponent(nullptr)
{
}

void CameraComponent::refreshProjectionMatrix()
{
	m_ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(m_FoV, m_AspectRatio.x / m_AspectRatio.y, m_Near, m_Far);
}

void CameraComponent::refreshViewMatrix()
{
	const Matrix& absoluteTransform = m_TransformComponent->getLocalTransform() * m_TransformComponent->getParentAbsoluteTransform();
	Vector3 target;
	Vector3::Transform(absoluteTransform.Forward(), absoluteTransform, target);
	m_ViewMatrix = Matrix::CreateLookAt(
	    absoluteTransform.Translation(),
	    target,
	    absoluteTransform.Up());
}

void CameraComponent::onRemove()
{
	if (m_Active)
	{
		RenderSystem::GetSingleton()->restoreCamera();
	}
}

bool CameraComponent::setup()
{
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			return false;
		}

		refreshProjectionMatrix();
		refreshViewMatrix();

		return true;
	}
	return false;
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
	}
	ImGui::SameLine();
	if (ImGui::Button("Aspect Ratio"))
	{
		m_AspectRatio = { 16.0f, 9.0f };
		refreshProjectionMatrix();
	}

	if (ImGui::SliderAngle("Field of View", &m_FoV, 1.0f, 180.0f))
	{
		refreshProjectionMatrix();
	}

	if (ImGui::DragFloatRange2("Range", &m_Near, &m_Far, 0.01f, 0.1f, 1000.0f))
	{
		refreshProjectionMatrix();
	}
}
#endif // ROOTEX_EDITOR
