#pragma once

#include "component.h"
#include "components/transform_component.h"

struct PostProcessingDetails
{
	bool isPostProcessing = false;
	bool isASSAO = false;
	bool isBloom = false;
	bool isSepia = false;
	bool isMonochrome = false;
	bool isGaussianBlur = false;
	bool isToneMap = false;
	float assaoRadius = 1.2f;
	float assaoDetailShadowStrength = 0.5f;
	int assaoBlurPassCount = 2;
	float assaoFadeOutFrom = 50.0f;
	float assaoFadeOutTo = 300.0f;
	float assaoHorizonAngleThreshold = 0.06f;
	int assaoQualityLevel = 2;
	float assaoShadowClamp = 0.98f;
	float assaoShadowMultiplier = 1.0f;
	float assaoShadowPower = 1.5f;
	float assaoSharpness = 0.98f;
	float assaoAdaptiveQualityLimit = 0.45f;
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
	DEPENDS_ON(TransformComponent);

	static Component* Create(const JSON::json& componentData);

	bool m_Active;
	float m_FoV;
	float m_Near;
	float m_Far;
	PostProcessingDetails m_PostProcessingDetails;

	Vector2 m_AspectRatio;
	Matrix m_ViewMatrix;
	Matrix m_ProjectionMatrix;

	CameraComponent(const Vector2& aspectRatio, float fov, float nearPlane, float farPlane, const PostProcessingDetails& postProcesing);
	CameraComponent(CameraComponent&) = delete;
	~CameraComponent() = default;

	friend class ECSFactory;

	void refreshProjectionMatrix();
	void refreshViewMatrix();

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::CameraComponent;

	virtual bool setupData() override;
	void onRemove() override;

	virtual Matrix& getViewMatrix();
	virtual Matrix& getProjectionMatrix();
	Vector3 getAbsolutePosition() const { return m_TransformComponent->getAbsoluteTransform().Translation(); }
	virtual const char* getName() const override { return "CameraComponent"; }

	PostProcessingDetails getPostProcessingDetails() const { return m_PostProcessingDetails; }

	ComponentID getComponentID() const { return s_ID; }

	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
