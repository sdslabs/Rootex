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

public:
	CameraVisualComponent();
	CameraVisualComponent(CameraVisualComponent&) = delete;
	~CameraVisualComponent();

	virtual bool preRender(VisualComponentGraph* visualComponentGraph) override;
	virtual void render(VisualComponentGraph* visualComponentGraph) override;
	virtual bool load(VisualComponentGraph* visualComponentGraph) override;
	virtual bool reset(VisualComponentGraph* visualComponentGraph, int windowWidth, int windowHeight);
	virtual bool isVisible(VisualComponentGraph* visualComponentGraph) const override;
	virtual void postRender(VisualComponentGraph* visualComponentGraph) override;

	virtual void setViewTransform(const Matrix& view);
	virtual void setTarget(Ref<VisualComponent> target) { m_Target = target; }

	virtual VisualComponent* getTarget() const { return m_Target.get(); }
	virtual const Matrix& getView() const { return m_ViewMatrix; }
	virtual const Matrix& getProjection() const { return m_ProjectionMatrix; }
	void setOffset(const Vector4& offset) { m_CameraOffset = offset; }
};
