#include "camera_visual_component.h"

#include "entity_factory.h"

CameraVisualComponent::CameraVisualComponent()
    : VisualComponent(RenderPassMain, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 100.0f))
    , m_Active(true)
    , m_Target(nullptr)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_Position(0.0f, 0.0f, 4.0f)
    , m_Direction(0.0f, 0.0f, -1.0f)
{
}

CameraVisualComponent::~CameraVisualComponent()
{
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
	m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, { 0.0f, 1.0f, 0.0f });
}

void CameraVisualComponent::setViewTransform(const Matrix& view)
{
	m_ViewMatrix = view;
}
