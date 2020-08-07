#pragma once

#include "common/common.h"
#include "component.h"

class TransformComponent : public Component
{
public:
	struct Bounds
	{
		Vector3 m_LowerBounds;
		Vector3 m_HigherBounds;
	};

private:
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	struct TransformBuffer
	{
		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;
		Bounds m_Bounds;

		Matrix m_Transform;
	};
	TransformBuffer m_TransformBuffer;
	
	Matrix m_ParentAbsoluteTransform;
	bool m_LockScale = false;

	const TransformBuffer* getTransformBuffer() const { return &m_TransformBuffer; };

	void updateTransformFromPositionRotationScale();
	void updatePositionRotationScaleFromTransform(Matrix& transform);

	TransformComponent(const Vector3& position, const Vector4& rotation, const Vector3& scale, const Vector3& lowerBounds, const Vector3& higherBounds);
	TransformComponent(TransformComponent&) = delete;

	friend class ModelComponent;
	friend class RenderSystem;
	friend class EntityFactory;

#ifdef ROOTEX_EDITOR
	static inline const float s_EditorDecimalSpeed = 0.01f;

	Vector3 m_EditorRotation;
#endif // ROOTEX_EDITOR

public:
	static void RegisterAPI(sol::state& rootex);

	static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformComponent;

	virtual ~TransformComponent() = default;

	void setPosition(const Vector3& position);
	void setRotation(const float& yaw, const float& pitch, const float& roll);
	void setRotationQuaternion(const Quaternion& rotation);
	void setScale(const Vector3& scale);
	void setTransform(const Matrix& transform);
	void setBounds(const Bounds& bounds);
	void setRotationPosition(const Matrix& transform);
	
	void addTransform(const Matrix& applyTransform);
	void addRotation(const Quaternion& applyTransform);

	Vector3 getPosition() const { return m_TransformBuffer.m_Position; }
	Bounds getBounds() const { return m_TransformBuffer.m_Bounds; }
	Bounds& getBoundsMutable() { return m_TransformBuffer.m_Bounds; }
	const Quaternion& getRotation() const { return m_TransformBuffer.m_Rotation; }
	const Vector3& getScale() const { return m_TransformBuffer.m_Scale; }
	const Matrix& getLocalTransform() const { return m_TransformBuffer.m_Transform; }
	Matrix getRotationPosition() const { return Matrix::CreateFromQuaternion(m_TransformBuffer.m_Rotation) * Matrix::CreateTranslation(m_TransformBuffer.m_Position) * m_ParentAbsoluteTransform; }
	Matrix getAbsoluteTransform() const { return m_TransformBuffer.m_Transform * m_ParentAbsoluteTransform; }
	Matrix getParentAbsoluteTransform() const { return m_ParentAbsoluteTransform; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "TransformComponent"; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
