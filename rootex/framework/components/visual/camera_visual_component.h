#pragma once

#include "visual_component.h"

class CameraVisualComponent : public VisualComponent
{
protected:
	Matrix m_ViewMatrix;
	Matrix m_ProjectionMatrix;
	bool m_Active;
	bool m_DebugCamera;
	Ref<VisualComponent> m_Target;
	Vector4 m_CameraOffset;
	Vector3 m_Position;
	Vector3 m_Direction;

public:
	CameraVisualComponent();
	CameraVisualComponent(CameraVisualComponent&) = delete;
	~CameraVisualComponent();

	virtual bool preRender() override;
	virtual void render() override;
	virtual bool reset(HierarchyGraph* visualComponentGraph, int windowWidth, int windowHeight);
	virtual bool isVisible() const override;
	virtual void postRender() override;

	virtual void setViewTransform(const Matrix& view);
	void setPosition(Vector3 position);
	virtual void setTarget(Ref<VisualComponent> target) { m_Target = target; }

	virtual VisualComponent* getTarget() const { return m_Target.get(); }
	virtual const Matrix& getView() const { return m_ViewMatrix; }
	virtual const Matrix& getProjection() const { return m_ProjectionMatrix; }
	void setOffset(const Vector4& offset) { m_CameraOffset = offset; }
	virtual String getName() const override { return "CameraVisualComponent"; }
};
