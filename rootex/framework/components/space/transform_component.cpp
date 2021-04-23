#include "transform_component.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "entity.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(TransformComponent);

TransformComponent::TransformComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_TransformBuffer(
          { data.value("position", Vector3::Zero),
              data.value("rotation", Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f)),
              data.value("scale", Vector3 { 1.0f, 1.0f, 1.0f }),
              data.value("boundingBox", BoundingBox { Vector3::Zero, Vector3 { 0.5f, 0.5f, 0.5f } }),
              Matrix::Identity })
    , m_TransformPassDown(data.value("passDown", (int)TransformPassDown::All))
    , m_OverrideBoundingBox(data.value("overrideBoundingBox", false))
{
	updateTransformFromPositionRotationScale();
}

void TransformComponent::updateAbsoluteTransformValues()
{
	m_AbsoluteTransform = m_TransformBuffer.transform * m_ParentAbsoluteTransform;
	m_AbsoluteTransform.Decompose(m_AbsoluteScale, m_AbsoluteRotation, m_AbsolutePosition);
}

void TransformComponent::updateTransformFromPositionRotationScale()
{
	m_TransformBuffer.transform = Matrix::Identity;
	m_TransformBuffer.transform = Matrix::CreateTranslation(m_TransformBuffer.position) * m_TransformBuffer.transform;
	m_TransformBuffer.transform = Matrix::CreateFromQuaternion(m_TransformBuffer.rotation) * m_TransformBuffer.transform;
	m_TransformBuffer.transform = Matrix::CreateScale(m_TransformBuffer.scale) * m_TransformBuffer.transform;
}

void TransformComponent::updatePositionRotationScaleFromTransform(Matrix& transform)
{
	transform.Decompose(m_TransformBuffer.scale, m_TransformBuffer.rotation, m_TransformBuffer.position);
}

void TransformComponent::setPosition(const Vector3& position)
{
	m_TransformBuffer.position = position;
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setAbsolutePosition(const Vector3& position)
{
	setPosition(position - getParentAbsoluteTransform().Translation());
}

void TransformComponent::setRotation(const float& yaw, const float& pitch, const float& roll)
{
	m_TransformBuffer.rotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setRotationQuaternion(const Quaternion& rotation)
{
	m_TransformBuffer.rotation = rotation;
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setScale(const Vector3& scale)
{
	m_TransformBuffer.scale = scale;
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setLocalTransform(const Matrix& transform)
{
	m_TransformBuffer.transform = transform;
	updatePositionRotationScaleFromTransform(m_TransformBuffer.transform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setAbsoluteTransform(const Matrix& transform)
{
	setLocalTransform(transform * m_ParentAbsoluteTransform.Invert());
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setBounds(const BoundingBox& bounds)
{
	if (!m_OverrideBoundingBox)
	{
		m_TransformBuffer.boundingBox = bounds;
	}
}

void TransformComponent::setRotationPosition(const Matrix& transform)
{
	m_TransformBuffer.transform = Matrix::CreateScale(m_TransformBuffer.scale) * transform;
	updatePositionRotationScaleFromTransform(m_TransformBuffer.transform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setAbsoluteRotationPosition(const Matrix& transform)
{
	setAbsoluteTransform(Matrix::CreateScale(m_TransformBuffer.scale) * transform);
	updatePositionRotationScaleFromTransform(m_TransformBuffer.transform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::setParentAbsoluteTransform(const Matrix& parentTransform)
{
	m_ParentAbsoluteTransform = parentTransform;
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::addLocalTransform(const Matrix& applyTransform)
{
	setLocalTransform(getLocalTransform() * applyTransform);
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::addQuaternion(const Quaternion& applyQuaternion)
{
	m_TransformBuffer.rotation = Quaternion::Concatenate(applyQuaternion, m_TransformBuffer.rotation);
	updateTransformFromPositionRotationScale();
	m_IsAbsoluteTransformDirty = true;
}

void TransformComponent::addRotation(float yaw, float pitch, float roll)
{
	addQuaternion(Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll));
}

BoundingBox TransformComponent::getWorldSpaceBounds()
{
	BoundingBox transformedBox = m_TransformBuffer.boundingBox;
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

	j["position"] = m_TransformBuffer.position;
	j["rotation"] = m_TransformBuffer.rotation;
	j["scale"] = m_TransformBuffer.scale;
	j["boundingBox"] = m_TransformBuffer.boundingBox;
	j["overrideBoundingBox"] = m_OverrideBoundingBox;
	j["passDown"] = m_TransformPassDown;

	return j;
}

void TransformComponent::draw()
{
	highlight();

	if (ImGui::DragFloat3("##Position", &m_TransformBuffer.position.x, 0.01f))
	{
		setPosition(m_TransformBuffer.position);
	}
	ImGui::SameLine();
	if (ImGui::Button("Position"))
	{
		setPosition({ 0.0f, 0.0f, 0.0f });
	}

	if (ImGui::DragFloat4("##Rotation", &m_TransformBuffer.rotation.x, 0.01f))
	{
		setRotationQuaternion(m_TransformBuffer.rotation);
	}

	ImGui::SameLine();
	if (ImGui::Button("Rotation"))
	{
		setRotation(0.0f, 0.0f, 0.0f);
	}

	static bool lockedScale = false;
	Vector3 savedScale = m_TransformBuffer.scale;
	if (ImGui::DragFloat3("##Scale", &m_TransformBuffer.scale.x, 0.1f, 0.001f, 10000.0f))
	{
		if (lockedScale && savedScale != Vector3::Zero)
		{
			Vector3 proportions = Vector3((m_TransformBuffer.scale - savedScale)) / savedScale;
			float ratio = proportions.x + proportions.y + proportions.z;
			setScale(savedScale * (1.0f + ratio));
		}
		else
		{
			setScale(m_TransformBuffer.scale);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Scale"))
	{
		setScale({ 1.0f, 1.0f, 1.0f });
	}
	ImGui::Checkbox("Lock Scale", &lockedScale);

	ImGui::Checkbox("Override bounds", &m_OverrideBoundingBox);

	if (m_OverrideBoundingBox)
	{
		ImGui::DragFloat3("##Center", &m_TransformBuffer.boundingBox.Center.x, 0.01f, 0.0f, FLT_MAX);
		ImGui::SameLine();
		if (ImGui::Button("Center"))
		{
			m_TransformBuffer.boundingBox.Center = Vector3::Zero;
		}

		ImGui::DragFloat3("##Extents", &m_TransformBuffer.boundingBox.Extents.x, 0.01f, 0.0f, FLT_MAX);
		ImGui::SameLine();
		if (ImGui::Button("Extents"))
		{
			m_TransformBuffer.boundingBox.Extents = 0.5f * Vector3::One;
		}
	}

	if (ImGui::TreeNode("Pass Down"))
	{
		ImGui::CheckboxFlags("All", &m_TransformPassDown, (int)TransformPassDown::All);
		ImGui::CheckboxFlags("Position", &m_TransformPassDown, (int)TransformPassDown::Position);
		ImGui::CheckboxFlags("Rotation", &m_TransformPassDown, (int)TransformPassDown::Rotation);
		ImGui::CheckboxFlags("Scale", &m_TransformPassDown, (int)TransformPassDown::Scale);

		ImGui::TreePop();
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

void to_json(JSON::json& j, const TransformPassDown& t)
{
	j = (int)t;
}

void from_json(const JSON::json& j, TransformPassDown& t)
{
	t = (TransformPassDown)(int)j;
}
