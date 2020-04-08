#include "camera_visual_component.h"
#include "systems/render_system.h"
Component* CameraVisualComponent::Create(const JSON::json& componentData)
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { componentData["position"]["x"], componentData["position"]["y"], componentData["position"]["z"] },
	    { componentData["direction"]["x"], componentData["direction"]["y"], componentData["direction"]["z"] },
	    { componentData["up"]["x"], componentData["up"]["y"], componentData["up"]["z"] });

	return cameraVisualComponent;
}

Component* CameraVisualComponent::CreateDefault()
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { 0.0f, 0.0f, 4.0f },
	    { 0.0f, 0.0f, -1.0f },
	    { 0.0f, 1.0f, 0.0f });
	
	return cameraVisualComponent;
}

CameraVisualComponent::CameraVisualComponent(const Vector3& position,const Vector3& direction, const Vector3& up)
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt(position, position + direction, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 100.0f))
    , m_Active(false)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_Position(position)
    , m_Direction(direction)
    , m_Up(up)
{
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>();
	}
}

CameraVisualComponent::CameraVisualComponent()
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt({ 0.0f, 0.0f, 0.4f }, {0.0f, 0.0f, 0.3f}, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 100.0f))
    , m_Active(false)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_Position(0.0f, 0.0f, 4.0f)
    , m_Direction(0.0f, 0.0f, -1.0f)
    , m_Up(0.0f, 1.0f, 0.0f)
{
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>();
	}
}

CameraVisualComponent::~CameraVisualComponent()
{
}

void CameraVisualComponent::onRemove()
{
	if (m_Active)
	{
		if (RenderSystem::GetSingleton() != nullptr)
		{
			RenderSystem::GetSingleton()->restoreCamera();
		}
	}
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

void CameraVisualComponent::setPosition(Vector3 position)
{
	m_Position = position;
	m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, m_Up);
}

void CameraVisualComponent::updatePosition()
{
	if (m_Owner)
	{
		if (m_TransformComponent == nullptr)
		{
			m_TransformComponent = m_Owner->getComponent<TransformComponent>();
		}
		m_Position = m_TransformComponent->getPosition();
		auto m_Rotation = m_TransformComponent->getRotation();
		if (m_Rotation.x != 0 || m_Rotation.y != 0 || m_Rotation.z != 0)
		{
			Quaternion inverse = Quaternion();
			inverse.x = -m_Rotation.x;
			inverse.y = -m_Rotation.y;
			inverse.z = -m_Rotation.z;
			inverse.w = m_Rotation.w;
			
			Quaternion direction = Quaternion();
			direction.x = 0;
			direction.y = 0;
			direction.z = -1;
			direction.w = 0;
			direction = inverse * direction * m_Rotation;
			m_Direction.x = direction.x;
			m_Direction.y = direction.y;
			m_Direction.z = direction.z;

			Quaternion up = Quaternion();
			up.x = 0;
			up.y = 1;
			up.z = 0;
			up.w = 0;
			up = inverse * up * m_Rotation;
			m_Up.x = up.x;
			m_Up.y = up.y;
			m_Up.z = up.z;
		}
		else
		{
			m_Up = { 0.0f, 1.0f, 0.0f };
			m_Direction = { 0.0f, 0.0f, -1.0f };
		}
		
	}
	m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, m_Up);
}

void CameraVisualComponent::setViewTransform(const Matrix& view)
{
	m_ViewMatrix = view;
}

void CameraVisualComponent::setNotActive()
{
	m_Active = false;
}

JSON::json CameraVisualComponent::getJSON() const
{
	JSON::json j;

	j["position"]["x"] = m_Position.x;
	j["position"]["y"] = m_Position.y;
	j["position"]["z"] = m_Position.z;

	j["direction"]["x"] = m_Direction.x;
	j["direction"]["y"] = m_Direction.y;
	j["direction"]["z"] = m_Direction.z;

	j["up"]["x"] = m_Up.x;
	j["up"]["y"] = m_Up.y;
	j["up"]["z"] = m_Up.z;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void CameraVisualComponent::draw()
{
	if (m_Active)
	{
		if (ImGui::Button("Set not active"))
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
			auto cameraPointer = m_Owner->getComponent<CameraVisualComponent>();
			RenderSystem::GetSingleton()->setCamera(cameraPointer);
		}
	}
}
#endif // ROOTEX_EDITOR
