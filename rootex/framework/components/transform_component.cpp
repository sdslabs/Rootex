#include "transform_component.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "entity.h"

Component* TransformComponent::Create(const JSON::json& componentData)
{
	TransformComponent* transformComponent = new TransformComponent(
	    { componentData["position"]["x"], componentData["position"]["y"], componentData["position"]["z"] },
	    { componentData["rotation"]["x"], componentData["rotation"]["y"], componentData["rotation"]["z"], componentData["rotation"]["w"] },
	    { componentData["scale"]["x"], componentData["scale"]["y"], componentData["scale"]["z"] });
	return transformComponent;
}

Component* TransformComponent::CreateDefault()
{
	TransformComponent* transformComponent = new TransformComponent(
	    { 0.0f, 0.0f, 0.0f },
	    Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f),
	    { 1.0f, 1.0f, 1.0f });
	return transformComponent;
}

void TransformComponent::updateTransformFromPositionRotationScale()
{
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform = Matrix::Identity;
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform = Matrix::CreateTranslation(m_TransformBuffer[PHYSICS_TRANSFROM].m_Position) * m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform;
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform = Matrix::CreateFromQuaternion(m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation) * m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform;
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform = Matrix::CreateScale(m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale) * m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform;
}

void TransformComponent::updatePositionRotationScaleFromTransform(Matrix& transform)
{
	transform.Decompose(m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale, m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation, m_TransformBuffer[PHYSICS_TRANSFROM].m_Position);
}

TransformComponent::TransformComponent(const Vector3& position, const Vector4& rotation, const Vector3& scale)
{
	m_TransformBuffer[0].m_Position = position;
	m_TransformBuffer[0].m_Rotation = rotation;
	m_TransformBuffer[0].m_Scale = scale;

	m_TransformBuffer[1].m_Position = position;
	m_TransformBuffer[1].m_Rotation = rotation;
	m_TransformBuffer[1].m_Scale = scale;

	updateTransformFromPositionRotationScale();
	exchangeBuffers();
	updateTransformFromPositionRotationScale();
	exchangeBuffers();

#ifdef ROOTEX_EDITOR
	m_EditorRotation = { 0.0f, 0.0f, 0.0f };
#endif // ROOTEX_EDITOR
}

void TransformComponent::setPosition(const Vector3& position)
{
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Position = position;
	updateTransformFromPositionRotationScale();
}

void TransformComponent::setRotation(const float& yaw, const float& pitch, const float& roll)
{
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	updateTransformFromPositionRotationScale();
}

void TransformComponent::setScale(const Vector3& scale)
{
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale = scale;
	updateTransformFromPositionRotationScale();
}

void TransformComponent::setTransform(const Matrix& transform)
{
	m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform = transform;
	updatePositionRotationScaleFromTransform(m_TransformBuffer[PHYSICS_TRANSFROM].m_Transform);
}

void TransformComponent::addTransform(const Matrix& applyTransform)
{
	setTransform(getLocalTransform() * applyTransform);
}

JSON::json TransformComponent::getJSON() const
{
	JSON::json j;

	j["position"]["x"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Position.x;
	j["position"]["y"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Position.y;
	j["position"]["z"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Position.z;

	j["rotation"]["x"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation.x;
	j["rotation"]["y"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation.y;
	j["rotation"]["z"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation.z;
	j["rotation"]["w"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation.w;

	j["scale"]["x"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale.x;
	j["scale"]["y"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale.y;
	j["scale"]["z"] = m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale.z;

	return j;
}

void TransformComponent::exchangeBuffers()
{
	PHYSICS_TRANSFROM = RENDERING_TRANSFROM; //The cooler way to write m_currentPhysicsBuffer = (m_currentPhysicsBuffer + 1) % 2
}

void TransformComponent::syncRenderingBuffer()
{
	m_TransformBuffer[RENDERING_TRANSFROM] = m_TransformBuffer[PHYSICS_TRANSFROM];
}

TransformBuffer* TransformComponent::getPhysicsBuffer()
{
	return &m_TransformBuffer[PHYSICS_TRANSFROM];
}

TransformBuffer* TransformComponent::getRenderingBuffer()
{
	return &m_TransformBuffer[RENDERING_TRANSFROM];
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void TransformComponent::draw()
{
	ImGui::DragFloat3("##P", &m_TransformBuffer[PHYSICS_TRANSFROM].m_Position.x, s_EditorDecimalSpeed);
	ImGui::SameLine();
	if (ImGui::Button("Position"))
	{
		m_TransformBuffer[PHYSICS_TRANSFROM].m_Position = { 0.0f, 0.0f, 0.0f };
	}

	if (ImGui::DragFloat3("##R", &m_EditorRotation.x, s_EditorDecimalSpeed))
	{
		m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation = Quaternion::CreateFromYawPitchRoll(m_EditorRotation.x, m_EditorRotation.y, m_EditorRotation.z);
	}
	ImGui::SameLine();
	if (ImGui::Button("Rotation"))
	{
		m_EditorRotation = { 0.0f, 0.0f, 0.0f };
		m_TransformBuffer[PHYSICS_TRANSFROM].m_Rotation = Quaternion::CreateFromYawPitchRoll(m_EditorRotation.x, m_EditorRotation.y, m_EditorRotation.z);
	}

	ImGui::DragFloat3("##S", &m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale.x, s_EditorDecimalSpeed, 0.0f, 0.0f);
	ImGui::SameLine();
	if (ImGui::Button("Scale"))
	{
		m_TransformBuffer[PHYSICS_TRANSFROM].m_Scale = { 1.0f, 1.0f, 1.0f };
	}

	updateTransformFromPositionRotationScale();
}
#endif // ROOTEX_EDITOR
