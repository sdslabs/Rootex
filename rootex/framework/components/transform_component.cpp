#include "transform_component.h"

Component* TransformComponent::Create(const LuaVariable& componentData)
{
	TransformComponent* transformComponent = new TransformComponent();

	transformComponent->m_TransformBuffer.m_Position.x = componentData["m_Position"]["x"];
	transformComponent->m_TransformBuffer.m_Position.y = componentData["m_Position"]["y"];
	transformComponent->m_TransformBuffer.m_Position.z = componentData["m_Position"]["z"];

	transformComponent->m_TransformBuffer.m_Rotation = Quaternion::CreateFromYawPitchRoll(
	    componentData["m_Rotation"]["yaw"],
	    componentData["m_Rotation"]["pitch"],
	    componentData["m_Rotation"]["roll"]);

	transformComponent->m_TransformBuffer.m_Scale.x = componentData["m_Scale"]["x"];
	transformComponent->m_TransformBuffer.m_Scale.y = componentData["m_Scale"]["y"];
	transformComponent->m_TransformBuffer.m_Scale.z = componentData["m_Scale"]["z"];

	transformComponent->updateTransformFromPositionRotationScale();

	return transformComponent;
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

TransformComponent::TransformComponent()
{
	m_TransformBuffer.m_Position = Vector3::Zero;
	m_TransformBuffer.m_Rotation = Vector4::Zero;
	m_TransformBuffer.m_Scale = Vector3::Zero;
	m_TransformBuffer.m_Transform = Matrix::Identity;
}

void TransformComponent::setPosition(const Vector3& position)
{
	m_TransformBuffer.m_Position = position;
	updateTransformFromPositionRotationScale();
}

void TransformComponent::setRotation(const float& yaw, const float& pitch, const float& roll)
{
	m_TransformBuffer.m_Rotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	updateTransformFromPositionRotationScale();
}

void TransformComponent::setScale(const Vector3& scale)
{
	m_TransformBuffer.m_Scale = scale;
	updateTransformFromPositionRotationScale();
}

void TransformComponent::setTransform(const Matrix& transform)
{
	m_TransformBuffer.m_Transform = transform;
	updatePositionRotationScaleFromTransform(m_TransformBuffer.m_Transform);
}

void TransformComponent::addTransform(const Matrix& applyTransform)
{
	setTransform(getLocalTransform() * applyTransform);
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void TransformComponent::draw()
{
	ImGui::DragFloat3("Position", &m_TransformBuffer.m_Position.x, s_EditorDecimalSpeed);
	ImGui::DragFloat3("Rotation", m_EditorRotation, s_EditorDecimalSpeed);
	ImGui::DragFloat3("Scale", &m_TransformBuffer.m_Scale.x, s_EditorDecimalSpeed);

	m_TransformBuffer.m_Rotation = Quaternion::CreateFromYawPitchRoll(m_EditorRotation[0], m_EditorRotation[1], m_EditorRotation[2]);
	updateTransformFromPositionRotationScale();
}
#endif // ROOTEX_EDITOR
