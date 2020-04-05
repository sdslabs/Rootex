#include "camera_visual_component.h"
#include "systems/render_system.h"
Component* CameraVisualComponent::Create(const JSON::json& componentData)
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { componentData["position"]["x"], componentData["position"]["y"], componentData["position"]["z"] },
	    { componentData["direction"]["x"], componentData["direction"]["y"], componentData["direction"]["z"] });

	Ref<CameraVisualComponent> cameraPointer(cameraVisualComponent);

	RenderSystem::GetSingleton()->setCamera(cameraPointer);

	return cameraVisualComponent;
}

Component* CameraVisualComponent::CreateDefault()
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { 0.0f, 0.0f, 4.0f },
	    { 0.0f, 0.0f, -1.0f });
	return cameraVisualComponent;
}

CameraVisualComponent::CameraVisualComponent(const Vector3& position,const Vector3& direction)
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt(position, position + direction, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 100.0f))
    , m_Active(true)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_Position(position)
    , m_Direction(direction)
{
	if (m_Owner)
	{
		//m_TransformComponent = m_Owner->getComponent<TransformComponent>();
	}
}

CameraVisualComponent::CameraVisualComponent()
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt({ 0.0f, 0.0f, 0.4f }, {0.0f, 0.0f, 0.3f}, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 100.0f))
    , m_Active(true)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_Position(0.0f, 0.0f, 4.0f)
    , m_Direction(0.0f, 0.0f, -1.0f)
{
	if (m_Owner)
	{
		//m_TransformComponent = m_Owner->getComponent<TransformComponent>();
	}
}

CameraVisualComponent::~CameraVisualComponent()
{
}

bool CameraVisualComponent::preRender()
{
	if (m_TransformComponent)
	{
		m_Position = m_TransformComponent->getPosition();
		m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, { 0.0f, 1.0f, 0.0f });
	}
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
	m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, { 0.0f, 1.0f, 0.0f });
}

void CameraVisualComponent::updatePosition()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>();
	m_Position = m_TransformComponent->getPosition();
}

void CameraVisualComponent::setViewTransform(const Matrix& view)
{
	m_ViewMatrix = view;
}
