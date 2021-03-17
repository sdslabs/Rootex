#pragma once

#include "component.h"
#include "components/space/transform_component.h"
#include "core/renderer/post_processor.h"

class CameraComponent : public Component
{
	DEFINE_COMPONENT(CameraComponent);
	DEPENDS_ON(TransformComponent);

	bool m_Active;
	float m_FoV;
	float m_Near;
	float m_Far;
	PostProcessingDetails m_PostProcessingDetails;

	Vector2 m_AspectRatio;
	Matrix m_ViewMatrix;
	Matrix m_ProjectionMatrix;

	void refreshProjectionMatrix();
	void refreshViewMatrix();

public:
	CameraComponent(const Vector2& aspectRatio, float fov, float nearPlane, float farPlane, const PostProcessingDetails& postProcesing);
	~CameraComponent() = default;

	Matrix& getViewMatrix();
	Matrix& getProjectionMatrix();
	Vector3 getAbsolutePosition() const { return m_TransformComponent->getAbsoluteTransform().Translation(); }

	PostProcessingDetails getPostProcessingDetails() const { return m_PostProcessingDetails; }

	bool setupData() override;
	void onRemove() override;
	JSON::json getJSON() const override;
	void draw() override;
};
