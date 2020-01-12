#pragma once

#include "common/common.h"
#include "component.h"

class TransformComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	struct TransformBuffer
	{
		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;

		Matrix m_Transform;
	};
	TransformBuffer m_TransformBuffer;

	const TransformBuffer* getTransformBuffer() const { return &m_TransformBuffer; };

	void updateTransformFromPositionRotationScale();
	void updatePositionRotationScaleFromTransform(Matrix& transform);
	
	TransformComponent();
	virtual ~TransformComponent() = default;

	friend class VisualComponent;
	friend class VisualComponentAttributes;
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformComponent;

	void setPosition(const Vector3& position);
	void setRotation(const float& yaw, const float& pitch, const float& roll);
	void setScale(const Vector3& scale);
	void setTransform(const Matrix& transform);
	void addTransform(const Matrix& applyTransform);

	const Vector3& getPosition() const { return m_TransformBuffer.m_Position; }
	const Quaternion& getRotation() const { return m_TransformBuffer.m_Rotation; }
	const Vector3& getScale() const { return m_TransformBuffer.m_Scale; }
	const Matrix& getTransform() const { return m_TransformBuffer.m_Transform; }
	ComponentID getComponentID() const override { return s_ID; }
};
