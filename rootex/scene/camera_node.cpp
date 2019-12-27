#include "camera_node.h"

#include "entity_factory.h"
#include "scene.h"

CameraNode::CameraNode()
    : SceneNode(INVALID_ID, "Camera", DirectX::XMMatrixTranslationFromVector({ 0.0f, 0.0f, 4.0f }), nullptr, RenderPass::Global, Material())
    , m_DebugCamera(false)
    , m_ViewMatrix(DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(DirectX::XMMatrixPerspectiveLH(1.0f, 1.0f * 640.0f / 480.0f, 0.5f, 10.0f))
    , m_Active(true)
    , m_Target(nullptr)
    , m_CameraOffset({ 0.0f, 1.0f, -10.0f, 0.0f })
	{
	}

	CameraNode::~CameraNode()
	{
	}

	void CameraNode::render(Scene * scene)
	{
		if (m_DebugCamera)
		{
			//render(m_Frustum);
		}
	}

	bool CameraNode::load(Scene * scene)
	{
		return true;
	}

	bool CameraNode::reset(Scene * scene, int windowWidth, int windowHeight)
	{
		// TODO: Add window resize logic that set the perspective matrix and frustum correctly

		return false;
	}

	bool CameraNode::isVisible(Scene * scene) const
	{
		return false;
	}

	void CameraNode::setViewTransform(const AlignedMatrix& view)
	{
		m_ViewMatrix = view;
	}
