#pragma once

#include "component.h"
#include "components/transform_component.h"

struct PostProcessingDetails
{
	bool isPostProcessing = false;
	bool isBloom = false;
	bool isSepia = false;
	bool isMonochrome = false;
	bool isGaussianBlur = false;
	bool isToneMap = false;
	float bloomThreshold = 0.8f;
	float bloomSize = 1.0f;
	float bloomBrightness = 1.0f;
	float bloomValue = 1.0f;
	float bloomBase = 1.0f;
	float bloomSaturation = 1.0f;
	float bloomBaseSaturation = 1.0f;
	float gaussianBlurMultiplier = 1.0f;
	float toneMapExposure = 0.0f;
	int toneMapOperator = 0;
	int toneMapTransferFunction = 0;
	float toneMapWhiteNits = 200.0f;
};

class CameraComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	bool m_Active;
	float m_FoV;
	float m_Near;
	float m_Far;
	PostProcessingDetails m_PostProcessingDetails;

	Vector2 m_AspectRatio;
	Matrix m_ViewMatrix;
	Matrix m_ProjectionMatrix;
	TransformComponent* m_TransformComponent;

	CameraComponent(const Vector2& aspectRatio, float fov, float nearPlane, float farPlane, const PostProcessingDetails& postProcesing);
	CameraComponent(CameraComponent&) = delete;
	~CameraComponent() = default;

	friend class EntityFactory;

	void refreshProjectionMatrix();
	void refreshViewMatrix();

public:
	virtual bool setup() override;
	void onRemove() override;

	TransformComponent* getTransformComponent() { return m_TransformComponent; }
	virtual const Matrix& getViewMatrix();
	virtual const Matrix& getProjectionMatrix();
	Vector3 getAbsolutePosition() const { return m_TransformComponent->getAbsoluteTransform().Translation(); }
	virtual String getName() const override { return "CameraComponent"; }

	PostProcessingDetails getPostProcessingDetails() const { return m_PostProcessingDetails; }

	static const ComponentID s_ID = (ComponentID)ComponentIDs::CameraComponent;
	ComponentID getComponentID() const { return s_ID; }

	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
