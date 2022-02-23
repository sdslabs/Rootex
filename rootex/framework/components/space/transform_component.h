#pragma once

#include "common/common.h"
#include "component.h"

enum class TransformPassDown : int
{
	Position = 1 << 0,
	Rotation = 1 << 1,
	Scale = 1 << 2,
	All = Position | Rotation | Scale
};

void to_json(JSON::json& j, const TransformPassDown& t);
void from_json(const JSON::json& j, TransformPassDown& t);

/// Stores the transform (position, rotation, scale) information of any entity.
class TransformComponent : public Component
{
	COMPONENT(TransformComponent, Category::General);

	struct TransformBuffer
	{
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;
		BoundingBox boundingBox;

		Matrix transform;
	};
	TransformBuffer m_TransformBuffer;
	int m_TransformPassDown;
	Matrix m_ParentAbsoluteTransform;

	Matrix m_AbsoluteTransform;
	Vector3 m_AbsolutePosition;
	Quaternion m_AbsoluteRotation;
	Vector3 m_AbsoluteScale;
	bool m_IsAbsoluteTransformDirty = true;
	bool m_OverrideBoundingBox;

	const TransformBuffer* getTransformBuffer() const { return &m_TransformBuffer; };

	void updateAbsoluteTransformValues();
	void updateTransformFromPositionRotationScale();
	void updatePositionRotationScaleFromTransform(Matrix& transform);

	friend class ModelComponent;
	friend class RenderSystem;

public:
	TransformComponent(Entity& owner, const JSON::json& data);
	~TransformComponent() = default;

	void setPosition(const Vector3& position);
	void setRotation(const float& yaw, const float& pitch, const float& roll);
	void setRotationQuaternion(const Quaternion& rotation);
	void setScale(const Vector3& scale);
	void setLocalTransform(const Matrix& transform);
	void setAbsoluteTransform(const Matrix& transform);
	void setBounds(const BoundingBox& bounds);
	void setRotationPosition(const Matrix& transform);
	void setAbsoluteRotationPosition(const Matrix& transform);
	void setParentAbsoluteTransform(const Matrix& parentTransform);

	void addLocalTransform(const Matrix& applyTransform);
	void addQuaternion(const Quaternion& applyQuaternion);
	void addRotation(float yaw, float pitch, float roll);

	Vector3 getPosition() const { return m_TransformBuffer.position; }
	Quaternion getRotation() const { return m_TransformBuffer.rotation; };
	const Vector3& getScale() const { return m_TransformBuffer.scale; }
	const Matrix& getLocalTransform() const { return m_TransformBuffer.transform; }
	Matrix getRotationPosition() const { return Matrix::CreateFromQuaternion(m_TransformBuffer.rotation) * Matrix::CreateTranslation(m_TransformBuffer.position) * m_ParentAbsoluteTransform; }
	Matrix getParentAbsoluteTransform() const { return m_ParentAbsoluteTransform; }
	int getPassDowns() const { return m_TransformPassDown; }

	BoundingBox getWorldSpaceBounds();

	Matrix getAbsoluteTransform();
	Vector3 getAbsolutePosition();
	void setAbsolutePosition(const Vector3& position);
	Quaternion getAbsoluteRotation();
	Vector3 getAbsoluteScale();

	JSON::json getJSON() const override;
	void draw() override;
	void highlight();
};

DECLARE_COMPONENT(TransformComponent);
