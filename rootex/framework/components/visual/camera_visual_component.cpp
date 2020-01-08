#include "camera_visual_component.h"

#include "visual_component_graph.h"
#include "entity_factory.h"

CameraVisualComponent::CameraVisualComponent()
    : VisualComponent(RenderPass::Global, nullptr, nullptr)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 10.0f))
    , m_Active(true)
    , m_Target(nullptr)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
{
}

CameraVisualComponent::~CameraVisualComponent()
{
}

bool CameraVisualComponent::preRender(VisualComponentGraph* visualComponentGraph)
{
	return true;
}

void CameraVisualComponent::render(VisualComponentGraph* visualComponentGraph)
{
	if (m_DebugCamera)
	{
		//render(m_Frustum);
	}
}

bool CameraVisualComponent::load(VisualComponentGraph* visualComponentGraph)
{
	return true;
}

bool CameraVisualComponent::reset(VisualComponentGraph* scene, int windowWidth, int windowHeight)
{
	// TODO: Add window resize logic that set the perspective matrix and frustum correctly

	return false;
}

bool CameraVisualComponent::isVisible(VisualComponentGraph* scene) const
{
	return false;
}

void CameraVisualComponent::postRender(VisualComponentGraph* visualComponentGraph)
{
}

void CameraVisualComponent::setViewTransform(const Matrix& view)
{
	m_ViewMatrix = view;
}
