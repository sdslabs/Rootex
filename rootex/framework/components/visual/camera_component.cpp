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
	}

	return true;
}

const Matrix& CameraComponent::getViewMatrix()
{
	Vector3& position = m_TransformComponent->getPosition();
	Matrix& transform = m_TransformComponent->getAbsoluteTransform();
	const Quaternion& rotation = m_TransformComponent->getRotation();
	Vector3& direction = transform.Forward();
	Vector3& up = transform.Up();
	if (rotation.x != 0 || rotation.y != 0 || rotation.z != 0)
	{
		direction = XMVector3Rotate(direction, rotation);
		up = XMVector3Rotate(up, rotation);
	}
	m_ViewMatrix = Matrix::CreateLookAt(position, position + direction, up);
	return m_ViewMatrix;
}

const Matrix& CameraComponent::getProjectionMatrix()
{
	m_ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(m_FoV, m_AspectRatio.x / m_AspectRatio.y, m_Near, m_Far);
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
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();
	}
	ImGui::SameLine();
	if (ImGui::Button("Aspect Ratio"))
	{
		m_AspectRatio = { 16.0f, 9.0f };
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();		
	}

	if (ImGui::SliderAngle("Field of View", &m_FoV, 1.0f, 180.0f))
	{
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();
	}

	if (ImGui::DragFloatRange2("Range", &m_Near, &m_Far, 0.01f, 0.1f, 1000.0f))
	{
		RenderSystem::GetSingleton()->setProjectionConstantBuffers();
	}
}
#endif // ROOTEX_EDITOR
