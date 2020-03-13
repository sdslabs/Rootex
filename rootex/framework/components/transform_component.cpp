#include "transform_component.h"

Component* TransformComponent::Create(const JSON::json& componentData)
{
	TransformComponent* transformComponent = new TransformComponent();

	transformComponent->m_TransformBuffer.m_Position.x = componentData["position"]["x"];
	transformComponent->m_TransformBuffer.m_Position.y = componentData["position"]["y"];
	transformComponent->m_TransformBuffer.m_Position.z = componentData["position"]["z"];

	transformComponent->m_TransformBuffer.m_Rotation.x = componentData["rotation"]["x"];
	transformComponent->m_TransformBuffer.m_Rotation.y = componentData["rotation"]["y"];
	transformComponent->m_TransformBuffer.m_Rotation.z = componentData["rotation"]["z"];
	transformComponent->m_TransformBuffer.m_Rotation.w = componentData["rotation"]["w"];

	transformComponent->m_TransformBuffer.m_Scale.x = componentData["scale"]["x"];
	transformComponent->m_TransformBuffer.m_Scale.y = componentData["scale"]["y"];
	transformComponent->m_TransformBuffer.m_Scale.z = componentData["scale"]["z"];

	transformComponent->updateTransformFromPositionRotationScale();

	return transformComponent;
}

Component* TransformComponent::CreateDefault()
{
	TransformComponent* transformComponent = new TransformComponent();

	// Position and Rotation will default to zero vectors here
	// Only Scale needs a default value of (1, 1, 1)
	transformComponent->m_TransformBuffer.m_Scale = { 1.0f, 1.0f, 1.0f };

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

JSON::json TransformComponent::getJSON() const
{
	JSON::json j;

	j["position"]["x"] = m_TransformBuffer.m_Position.x;
	j["position"]["y"] = m_TransformBuffer.m_Position.y;
	j["position"]["z"] = m_TransformBuffer.m_Position.z;

	j["rotation"]["x"] = m_TransformBuffer.m_Rotation.x;
	j["rotation"]["y"] = m_TransformBuffer.m_Rotation.y;
	j["rotation"]["z"] = m_TransformBuffer.m_Rotation.z;
	j["rotation"]["w"] = m_TransformBuffer.m_Rotation.w;

	j["scale"]["x"] = m_TransformBuffer.m_Scale.x;
	j["scale"]["y"] = m_TransformBuffer.m_Scale.y;
	j["scale"]["z"] = m_TransformBuffer.m_Scale.z;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void TransformComponent::draw()
{
	ImGui::DragFloat3("##P", &m_TransformBuffer.m_Position.x, s_EditorDecimalSpeed);
	ImGui::SameLine();
	if (ImGui::Button("Position"))
	{
		m_TransformBuffer.m_Position = { 0.0f, 0.0f, 0.0f };
	}

	ImGui::DragFloat3("##R", m_EditorRotation, s_EditorDecimalSpeed);
	ImGui::SameLine();
	if (ImGui::Button("Rotation"))
	{
		m_EditorRotation[0] = 0.0f;
		m_EditorRotation[1] = 0.0f;
		m_EditorRotation[2] = 0.0f;
		m_EditorRotation[3] = 0.0f;
	}
	m_TransformBuffer.m_Rotation = Quaternion::CreateFromYawPitchRoll(m_EditorRotation[0], m_EditorRotation[1], m_EditorRotation[2]);

	ImGui::DragFloat3("##S", &m_TransformBuffer.m_Scale.x, s_EditorDecimalSpeed, 0.0f, 0.0f);
	ImGui::SameLine();
	if (ImGui::Button("Scale"))
	{
		m_TransformBuffer.m_Scale = { 1.0f, 1.0f, 1.0f };
	}

	updateTransformFromPositionRotationScale();
}
#endif // ROOTEX_EDITOR
