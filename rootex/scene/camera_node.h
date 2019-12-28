#pragma once

#include <DirectXMath.h>

#include "common/types.h"
#include "scene_node.h"

class CameraNode : public SceneNode
{
protected:
	Matrix m_ViewMatrix;
	Matrix m_ProjectionMatrix;
	bool m_Active;
	bool m_DebugCamera;
	Ref<SceneNode> m_Target;
	Vector4 m_CameraOffset;

public:
	CameraNode();
	CameraNode(CameraNode&) = delete;
	~CameraNode();

	virtual void render(Scene* scene) override;
	virtual bool load(Scene* scene) override;
	virtual bool reset(Scene* scene, int windowWidth, int windowHeight);
	virtual bool isVisible(Scene* scene) const override;

	virtual void setViewTransform(const Matrix& view);
	virtual void setTarget(Ref<SceneNode> target) { m_Target = target; }

	virtual SceneNode* getTarget() const { return m_Target.get(); }
	virtual const Matrix& getView() const { return m_ViewMatrix; }
	virtual const Matrix& getProjection() const { return m_ProjectionMatrix; }
	void setOffset(const Vector4& offset) { m_CameraOffset = offset; }
};
