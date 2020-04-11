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
	TransformComponent* m_TransformComponent;
	Vector4 m_CameraOffset;
	Vector2 m_AspectRatio;
#ifdef ROOTEX_EDITOR
	static inline const float s_EditorDecimalSpeed = 0.01f;
#endif // ROOTEX_EDITOR

public:
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();
	CameraVisualComponent(const Vector2& aspectRatio);
	CameraVisualComponent();
	CameraVisualComponent(CameraVisualComponent&) = delete;
	~CameraVisualComponent();

	virtual bool setup() override;
	virtual bool preRender() override;
	virtual void render() override;
	virtual bool reset(HierarchyGraph* visualComponentGraph, int windowWidth, int windowHeight);
	virtual bool isVisible() const override;
	virtual void postRender() override;

	void onRemove() override;
	virtual void setViewTransform(const Matrix& view);
	void setInactive();
	void updatePosition();
	virtual void setTarget(Ref<VisualComponent> target) { m_Target = target; }

	virtual VisualComponent* getTarget() const { return m_Target.get(); }
	virtual const Matrix& getView();
	virtual const Matrix& getProjection() const { return m_ProjectionMatrix; }
	void setOffset(const Vector4& offset) { m_CameraOffset = offset; }
	virtual String getName() const override { return "CameraVisualComponent"; }

	static const ComponentID s_ID = (ComponentID)ComponentIDs::CameraVisualComponent;
	ComponentID getComponentID() const { return s_ID; }

	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
