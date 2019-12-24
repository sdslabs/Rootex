#include "camera_node.h"

#include "scene.h"
#include "entity_factory.h"

const AlignedMatrix& CameraNode::getView() const
{
	AlignedMatrix v =  DirectX::XMLoadFloat4x4(&m_ViewMatrix);
	return v;
}

CameraNode::CameraNode(AlignedMatrix transform)
    : SceneNode(INVALID_ID, "Camera", &transform, nullptr, RenderPass::Global, Material())
    , m_DebugCamera(false)
    , m_Frustum()
    , m_Active(true)
    , m_Target(nullptr)
    , m_CameraOffset({ 0.0f, 1.0f, -10.0f, 0.0f })
{
}

CameraNode::~CameraNode()
{
}

void CameraNode::render(Scene* scene)
{
	if (m_DebugCamera)
	{
		//render(m_Frustum);
	}
}

bool CameraNode::load(Scene* scene)
{
	return true;
}

bool CameraNode::reset(Scene* scene, int windowWidth, int windowHeight)
{
	// TODO: Add window resize logic that set the perspective matrix and frustum correctly

	return false;
}

bool CameraNode::isVisible(Scene* scene) const
{
	return false;
}

void CameraNode::setSceneView(Scene* scene)
{
	// TODO: Set view matrix from rendering here
}

void CameraNode::setViewTransform(const AlignedMatrix& view)
{
	DirectX::XMFLOAT4X4 newMatrix;
	DirectX::XMStoreFloat4x4(&newMatrix, view);
	m_ViewMatrix = newMatrix;
}
