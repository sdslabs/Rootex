#include "camera_visual_component.h"
#include "systems/render_system.h"
Component* CameraVisualComponent::Create(const JSON::json& componentData)
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { componentData["aspectRatio"]["x"], componentData["aspectRatio"]["y"]});

	return cameraVisualComponent;
}

Component* CameraVisualComponent::CreateDefault()
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { 16.0f, 9.0f });
	
	return cameraVisualComponent;
}

CameraVisualComponent::CameraVisualComponent(const Vector2& aspectRatio)
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt({ 0.0f, 0.0f, 0.4f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f*aspectRatio.y/aspectRatio.x, 0.5f, 100.0f))
    , m_Active(false)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_AspectRatio(aspectRatio)
    , m_TransformComponent(nullptr)
{
}

CameraVisualComponent::CameraVisualComponent()
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt({ 0.0f, 0.0f, 0.4f }, {0.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f*9.0f/16.0f, 0.5f, 100.0f))
    , m_Active(false)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_AspectRatio(16.0f, 9.0f)
    , m_TransformComponent(nullptr)
{
}

CameraVisualComponent::~CameraVisualComponent()
{
}

void CameraVisualComponent::onRemove()
{
	if (m_Active)
	{
		RenderSystem::GetSingleton()->restoreCamera();
	}
}
bool CameraVisualComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			status = false;
		}

		HierarchyComponent* hierarchyComponent = m_Owner->getComponent<HierarchyComponent>().get();
		if (hierarchyComponent == nullptr)
		{
			WARN("Entity without hierarchy component found");
			status = false;
		}
	}

	return status;
}
bool CameraVisualComponent::preRender()
{
	return true;
}

void CameraVisualComponent::render()
{
	if (m_DebugCamera)
	{
		//render(m_Frustum);
	}
}

bool CameraVisualComponent::reset(HierarchyGraph* scene, int windowWidth, int windowHeight)
{
	// TODO: Add window resize logic that set the perspective matrix and frustum correctly

	return false;
}

bool CameraVisualComponent::isVisible() const
{
	return m_IsVisible;
}

void CameraVisualComponent::postRender()
{
}

void CameraVisualComponent::updatePosition()
{
	if (m_TransformComponent)
	{
		Vector3 position = m_TransformComponent->getPosition();
		Matrix transform = m_TransformComponent->getAbsoluteTransform();
		Quaternion rotation = m_TransformComponent->getRotation();
		Vector3 direction = transform.Forward();
		Vector3 up = transform.Up();
		if (rotation.x != 0 || rotation.y != 0 || rotation.z != 0)
		{
			Quaternion inverse = Quaternion();
			inverse.x = -rotation.x;
			inverse.y = -rotation.y;
			inverse.z = -rotation.z;
			inverse.w = rotation.w;
			
			Quaternion directionQuaternion = Quaternion();
			directionQuaternion.x = direction.x;
			directionQuaternion.y = direction.y;
			directionQuaternion.z = direction.z;
			directionQuaternion.w = 0;
			directionQuaternion = inverse * directionQuaternion * rotation;
			direction = { directionQuaternion.x, directionQuaternion.y, directionQuaternion.z };

			Quaternion upQuaternion = Quaternion();
			upQuaternion.x = up.x;
			upQuaternion.y = up.y;
			upQuaternion.z = up.z;
			upQuaternion.w = 0;
			upQuaternion = inverse * upQuaternion * rotation;
			up = { upQuaternion.x, upQuaternion.y, upQuaternion.z };
		}
		m_ViewMatrix = Matrix::CreateLookAt(position, position + direction, up);
	}
}

void CameraVisualComponent::setViewTransform(const Matrix& view)
{
	m_ViewMatrix = view;
}

void CameraVisualComponent::setInactive()
{
	m_Active = false;
}

JSON::json CameraVisualComponent::getJSON() const
{
	JSON::json j;

	j["aspectRatio"]["x"] = m_AspectRatio.x;
	j["aspectRatio"]["y"] = m_AspectRatio.y;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void CameraVisualComponent::draw()
{
	ImGui::DragFloat2("##A", &m_AspectRatio.x, s_EditorDecimalSpeed);
	ImGui::SameLine();
	if (ImGui::Button("Aspect Ratio"))
	{
		m_AspectRatio = { 16.0f, 9.0f };
	}
	m_ProjectionMatrix = Matrix::CreatePerspective(1.0f, 1.0f * m_AspectRatio.y / m_AspectRatio.x, 0.5f, 100.0f);
	if (m_Active)
	{
		if (ImGui::Button("Set Inactive"))
		{
			m_Active = false;
			RenderSystem::GetSingleton()->restoreCamera();
		}
	}
	else
	{
		if (ImGui::Button("Set Active"))
		{
			m_Active = true;
			Ref<CameraVisualComponent> cameraPointer = m_Owner->getComponent<CameraVisualComponent>();
			RenderSystem::GetSingleton()->setCamera(cameraPointer);
		}
	}
}
#endif // ROOTEX_EDITOR
