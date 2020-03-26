#pragma once

#include "common/common.h"
#include "component.h"

#define PHYSICS_TRANSFROM m_currentPhysicsBuffer
#define RENDERING_TRANSFROM (m_currentPhysicsBuffer + 1) % 2

struct TransformBuffer
{
	Vector3 m_Position;
	Quaternion m_Rotation;
	Vector3 m_Scale;

	Matrix m_Transform;
	Matrix m_AbsoluteTransform;
};

class TransformComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	TransformBuffer m_TransformBuffer[2];
	int m_currentPhysicsBuffer = 0;

	const TransformBuffer* getTransformBuffer() const { return &m_TransformBuffer[PHYSICS_TRANSFROM]; };

	void updateTransformFromPositionRotationScale();
	void updatePositionRotationScaleFromTransform(Matrix& transform);

	TransformComponent(const Vector3& position, const Vector4& rotation, const Vector3& scale);
	TransformComponent(TransformComponent&) = delete;

	TransformBuffer* getPhysicsBuffer();
	TransformBuffer* getRenderingBuffer();
	void syncRenderingBuffer();


	friend class VisualComponent;
	friend class DiffuseVisualComponent;
	friend class VisualComponentAttributes;
	friend class RenderSystem;
	friend class PhysicsSystem;
	friend class EntityFactory;

#ifdef ROOTEX_EDITOR
	static inline const float s_EditorDecimalSpeed = 0.01f;

	Vector3 m_EditorRotation;
#endif // ROOTEX_EDITOR

public:
	//IF not mentioned Transform is assumed to be physics transform
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformComponent;

	virtual ~TransformComponent() = default;

	void setPosition(const Vector3& position);
	void setRotation(const float& yaw, const float& pitch, const float& roll);
	void setScale(const Vector3& scale);
	void setTransform(const Matrix& transform);
	void addTransform(const Matrix& applyTransform);

	Vector3 getPosition() const { return m_TransformBuffer[PHYSICS_TRANSFROM].m_Position; }
	const Quaternion& getRotation() const { return m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation; }
	const Vector3& getScale() const { return m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale; }
	const Matrix& getLocalTransform() const { return m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform; }
	Matrix getAbsoluteTransform() const { return m_TransformBuffer[PHYSICS_TRANSFROM].m_AbsoluteTransform; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "TransformComponent"; }
	virtual JSON::json getJSON() const override;
	void exchangeBuffers();

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
