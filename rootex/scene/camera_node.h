#pragma once

#include <DirectXCollision.h>

#include "common/types.h"
#include "scene_node.h"

class CameraNode : public SceneNode
{
protected:
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::BoundingFrustum m_Frustum;
	bool m_Active;
	bool m_DebugCamera;
	Ref<SceneNode> m_Target;
	AlignedVector m_CameraOffset;

public:
	CameraNode(AlignedMatrix transform);
	CameraNode(CameraNode&) = delete;
	~CameraNode();

	virtual void render(Scene* scene) override;
	virtual bool load(Scene* scene) override;
	virtual bool reset(Scene* scene, int windowWidth, int windowHeight);
	virtual bool isVisible(Scene* scene) const override;

	virtual void setFrustum(const DirectX::BoundingFrustum& frustum) { m_Frustum = frustum; }
	void setSceneView(Scene* scene);
	virtual void setViewTransform(const AlignedMatrix& view);
	virtual void setTarget(Ref<SceneNode> target) { m_Target = target; }

	virtual SceneNode* getTarget() const { return m_Target.get(); }
	virtual const AlignedMatrix& getView() const { return DirectX::XMLoadFloat4x4(&m_ViewMatrix); }

	void setOffset(const AlignedVector& offset) { m_CameraOffset = offset; }
};
