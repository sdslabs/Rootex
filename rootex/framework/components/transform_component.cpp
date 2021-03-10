#include "transform_component.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "entity.h"
#include "systems/render_system.h"

Component* TransformComponent::Create(const JSON::json& componentData)
{
	TransformComponent* transformComponent = new TransformComponent(
	    componentData.value("position", Vector3::Zero),
	    componentData.value("rotation", Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f)),
	    componentData.value("scale", Vector3 { 1.0f, 1.0f, 1.0f }),
	    componentData.value("boundingBox", BoundingBox { Vector3::Zero, Vector3 { 0.5f, 0.5f, 0.5f } }));
	return transformComponent;
}

void TransformComponent::updateAbsoluteTransformValues()
{
	m_AbsoluteTransform = m_TransformBuffer.m_Transform * m_ParentAbsoluteTransform;
	m_AbsoluteTransform.Decompose(m_AbsoluteScale, m_AbsoluteRotation, m_AbsolutePosition);
}

void TransformComponent::updateTransformFromPositionRotationScale()
{
	m_TransformBuffer.m_Transform = Matrix::Identity;
	m_TransformBuffer.m_Transform = Matrix::CreateTranslation(m_TransformBuffer.m_Position) * m_TransformBuffer.m_Transform;
	m_TransformBuffer.m_Transform = Matrix::CreateFromQuaternion(m_TransformBuffer.m_Rotation) * m_TransformBuffer.m_Transform;
	m_TransformBuffer.m_Transform = Matrix::CreateScale(m_TransformBuffer.m_Scale) * m_TransformBuffer.m_Transform;
}

void TransformComponent::updatePositionRotationScaleFromTransform(Matrix& transform)
{
	transform.Decompose(m_TransformBuffer.m_Scale, m_TransformBuffer.m_Rotation, m_TransformBuffer.m_Position);
}

TransformComponent::TransformComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const BoundingBox& bounds)
{
	m_TransformBuffer.m_Position = position;
	m_TransformBuffer.m_Rotation = rotation;
	m_TransformBuffer.m_Scale = scale;
	m_TransformBuffer.m_BoundingBox = bounds;

	updateTransformFromPositionRotationScale();
}

void TransformComponent::setPosition(const Vector3& position)
{
	m_TransformBuffer.m_Position = position;
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setRotation(const float& yaw, const float& pitch, const float& roll)
{
	m_TransformBuffer.m_Rotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setRotationQuaternion(const Quaternion& rotation)
{
	m_TransformBuffer.m_Rotation = rotation;
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setScale(const Vector3& scale)
{
	m_TransformBuffer.m_Scale = scale;
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setTransform(const Matrix& transform)
{
	m_TransformBuffer.m_Transform = transform;
	updatePositionRotationScaleFromTransform(m_TransformBuffer.m_Transform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setAbsoluteTransform(const Matrix& transform)
{
	setTransform(transform * m_ParentAbsoluteTransform.Invert());
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setBounds(const BoundingBox& bounds)
{
	m_TransformBuffer.m_BoundingBox = bounds;
}

void TransformComponent::setRotationPosition(const Matrix& transform)
{
	m_TransformBuffer.m_Transform = Matrix::CreateScale(m_TransformBuffer.m_Scale) * transform;
	updatePositionRotationScaleFromTransform(m_TransformBuffer.m_Transform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setAbsoluteRotationPosition(const Matrix& transform)
{
	setAbsoluteTransform(Matrix::CreateScale(m_TransformBuffer.m_Scale) * transform);
	updatePositionRotationScaleFromTransform(m_TransformBuffer.m_Transform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setParentAbsoluteTransform(const Matrix& parentTransform)
{
	m_ParentAbsoluteTransform = parentTransform;
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::addTransform(const Matrix& applyTransform)
{
	setTransform(getLocalTransform() * applyTransform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::addRotation(const Quaternion& applyTransform)
{
	m_TransformBuffer.m_Rotation = Quaternion::Concatenate(applyTransform, m_TransformBuffer.m_Rotation);
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

BoundingBox TransformComponent::getWorldSpaceBounds()
{
	BoundingBox transformedBox = m_TransformBuffer.m_BoundingBox;
	transformedBox.Transform(transformedBox, getAbsoluteTransform());
	return transformedBox;
}

Matrix TransformComponent::getAbsoluteTransform()
{
	if (m_IsAbsoluteTransformDirty)
	{
		updateAbsoluteTransformValues();
	}
	m_IsAbsoluteTransformDirty = false;
	return m_AbsoluteTransform;
}

Vector3 TransformComponent::getAbsolutePosition()
{
	if (m_IsAbsoluteTransformDirty)
	{
		updateAbsoluteTransformValues();
	}
	m_IsAbsoluteTransformDirty = false;
	return m_AbsolutePosition;
}

Quaternion TransformComponent::getAbsoluteRotation()
{
	if (m_IsAbsoluteTransformDirty)
	{
		updateAbsoluteTransformValues();
	}
	m_IsAbsoluteTransformDirty = false;
	return m_AbsoluteRotation;
}

Vector3 TransformComponent::getAbsoluteScale()
{
	if (m_IsAbsoluteTransformDirty)
	{
		updateAbsoluteTransformValues();
	}
	m_IsAbsoluteTransformDirty = false;
	return m_AbsoluteScale;
}

JSON::json TransformComponent::getJSON() const
{
	JSON::json j;

	j["position"] = m_TransformBuffer.m_Position;
	j["rotation"] = m_TransformBuffer.m_Rotation;
	j["scale"] = m_TransformBuffer.m_Scale;
	j["boundingBox"] = m_TransformBuffer.m_BoundingBox;

	return j;
}

void TransformComponent::draw()
{
	highlight();

	ImGui::DragFloat3("##Position", &m_TransformBuffer.m_Position.x, 0.01f);
	ImGui::SameLine();
	if (ImGui::Button("Position"))
	{
		m_TransformBuffer.m_Position = { 0.0f, 0.0f, 0.0f };
	}

	ImGui::DragFloat4("##Rotation", &m_TransformBuffer.m_Rotation.x, 0.01f);

	ImGui::SameLine();
	if (ImGui::Button("Rotation"))
	{
		setRotation(0.0f, 0.0f, 0.0f);
	}

	static bool lockedFirstFrame = false;
	if (m_LockScale)
	{
		static Vector3 lockedScale;
		static float scaleRatioYX;
		static float scaleRatioZX;
		if (!lockedFirstFrame)
		{
			lockedScale = m_TransformBuffer.m_Scale;
			scaleRatioYX = lockedScale.y / lockedScale.x;
			scaleRatioZX = lockedScale.z / lockedScale.x;
			lockedFirstFrame = true;
		}

		if (lockedScale.x - m_TransformBuffer.m_Scale.x)
		{
			lockedScale.y = lockedScale.x * scaleRatioYX;
			lockedScale.z = lockedScale.x * scaleRatioZX;
		}
		else if (lockedScale.y - m_TransformBuffer.m_Scale.y)
		{
			lockedScale.x = lockedScale.y / scaleRatioYX;
			lockedScale.z = lockedScale.y * scaleRatioZX / scaleRatioYX;
		}
		else if (lockedScale.z - m_TransformBuffer.m_Scale.z)
		{
			lockedScale.x = lockedScale.z / scaleRatioZX;
			lockedScale.y = lockedScale.z * scaleRatioYX / scaleRatioZX;
		}

		m_TransformBuffer.m_Scale = { lockedScale.x, lockedScale.y, lockedScale.z };
		ImGui::DragFloat3("##Scale", &lockedScale.x, 0.01f, 0.0f, 0.0f);
	}
	else
	{
		lockedFirstFrame = false;
		ImGui::DragFloat3("##Scale", &m_TransformBuffer.m_Scale.x, 0.01f, 0.0f, 0.0f);
	}

	ImGui::SameLine();
	if (ImGui::Button("Scale"))
	{
		m_TransformBuffer.m_Scale = { 1.0f, 1.0f, 1.0f };
	}

	ImGui::Checkbox("Lock Scale", &m_LockScale);

	ImGui::DragFloat3("##Center", &m_TransformBuffer.m_BoundingBox.Center.x, 0.01f);
	ImGui::SameLine();
	if (ImGui::Button("Center"))
	{
		m_TransformBuffer.m_BoundingBox.Center = { 0.0f, 0.0f, 0.0f };
	}

	ImGui::DragFloat3("##Bounds", &m_TransformBuffer.m_BoundingBox.Extents.x, 0.01f);
	ImGui::SameLine();
	if (ImGui::Button("Extents"))
	{
		m_Owner->onAllComponentsAdded();
	}

	updateTransformFromPositionRotationScale();
}

void TransformComponent::highlight()
{
	BoundingBox transformedBox = getWorldSpaceBounds();
	Vector3 min = Vector3(transformedBox.Center) - transformedBox.Extents;
	Vector3 max = Vector3(transformedBox.Center) + transformedBox.Extents;
	RenderSystem::GetSingleton()->submitBox(min, max);
	Vector3 forward;
	getAbsoluteTransform().Forward().Normalize(forward);
	RenderSystem::GetSingleton()->submitLine(transformedBox.Center, transformedBox.Center + (transformedBox.Extents.z * 2.0f) * forward);
}
