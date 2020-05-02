#include "animated_character_component.h"

Component* AnimatedCharacterComponent::Create(const JSON::json& componentData)
{
	return new AnimatedCharacterComponent();
}

Component* AnimatedCharacterComponent::CreateDefault()
{
	return new AnimatedCharacterComponent();
}

AnimatedCharacterComponent::AnimatedCharacterComponent()
    : VisualComponent(RenderPassMain, true)
    , m_CurrentTimePosition(0.0f)
{
	using namespace DirectX;

	XMVECTOR q0 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(30.0f));
	XMVECTOR q1 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 1.0f, 2.0f, 0.0f), XMConvertToRadians(45.0f));
	XMVECTOR q2 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(-30.0f));
	XMVECTOR q3 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(70.0f));
	m_BoneAnimation.m_Keyframes.resize(5);
	m_BoneAnimation.m_Keyframes[0].m_TimePosition = 0.0f;
	m_BoneAnimation.m_Keyframes[0].m_Translation = XMFLOAT3(-7.0f, 0.0f, 0.0f);
	m_BoneAnimation.m_Keyframes[0].m_Scale = XMFLOAT3(0.25f, 0.25f, 0.25f);
	XMStoreFloat4(&m_BoneAnimation.m_Keyframes[0].m_Rotation, q0);
	m_BoneAnimation.m_Keyframes[1].m_TimePosition = 2.0f;
	m_BoneAnimation.m_Keyframes[1].m_Translation = XMFLOAT3(0.0f, 2.0f, 10.0f);
	m_BoneAnimation.m_Keyframes[1].m_Scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	XMStoreFloat4(&m_BoneAnimation.m_Keyframes[1].m_Rotation, q1);
	m_BoneAnimation.m_Keyframes[2].m_TimePosition = 4.0f;
	m_BoneAnimation.m_Keyframes[2].m_Translation = XMFLOAT3(7.0f, 0.0f, 0.0f);
	m_BoneAnimation.m_Keyframes[2].m_Scale = XMFLOAT3(0.25f, 0.25f, 0.25f);
	XMStoreFloat4(&m_BoneAnimation.m_Keyframes[2].m_Rotation, q2);
	m_BoneAnimation.m_Keyframes[3].m_TimePosition = 6.0f;
	m_BoneAnimation.m_Keyframes[3].m_Translation = XMFLOAT3(0.0f, 1.0f, -10.0f);
	m_BoneAnimation.m_Keyframes[3].m_Scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	XMStoreFloat4(&m_BoneAnimation.m_Keyframes[3].m_Rotation, q3);
	m_BoneAnimation.m_Keyframes[4].m_TimePosition = 8.0f;
	m_BoneAnimation.m_Keyframes[4].m_Translation = XMFLOAT3(-7.0f, 0.0f, 0.0f);
	m_BoneAnimation.m_Keyframes[4].m_Scale = XMFLOAT3(0.25f, 0.25f, 0.25f);
	XMStoreFloat4(&m_BoneAnimation.m_Keyframes[4].m_Rotation, q0);
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AnimatedCharacterComponent::draw()
{
	ImGui::SliderFloat("Time", &m_CurrentTimePosition, 0.0f, 10.0f);
}
#endif // ROOTEX_EDITOR
