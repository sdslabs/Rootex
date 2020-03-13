#pragma once

#include "common/common.h"
#include "component.h"

class TransformComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	struct TransformBuffer
	{
		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;

		Matrix m_Transform;
		Matrix m_AbsoluteTransform;
	};
	TransformBuffer m_TransformBuffer;

	const TransformBuffer* getTransformBuffer() const { return &m_TransformBuffer; };

	void updateTransformFromPositionRotationScale();
	void updatePositionRotationScaleFromTransform(Matrix& transform);

	TransformComponent();

	friend class VisualComponent;
	friend class DiffuseVisualComponent;
	friend class VisualComponentAttributes;
	friend class EntityFactory;

#ifdef ROOTEX_EDITOR
	static inline const float s_EditorDecimalSpeed = 0.01f;

	float m_EditorRotation[3] = { 0.0f, 0.0f, 0.0f };
#endif // ROOTEX_EDITOR

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformComponent;
	
	virtual ~TransformComponent() = default;

	void setPosition(const Vector3& position);
	void setRotation(const float& yaw, const float& pitch, const float& roll);
	void setScale(const Vector3& scale);
	void setTransform(const Matrix& transform);
	void addTransform(const Matrix& applyTransform);

	Vector3 getPosition() const { return m_TransformBuffer.m_Position; }
	const Quaternion& getRotation() const { return m_TransformBuffer.m_Rotation; }
	const Vector3& getScale() const { return m_TransformBuffer.m_Scale; }
	const Matrix& getLocalTransform() const { return m_TransformBuffer.m_Transform; }
	Matrix getAbsoluteTransform() const { return m_TransformBuffer.m_AbsoluteTransform; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "TransformComponent"; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
