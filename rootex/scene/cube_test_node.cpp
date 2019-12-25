#include "cube_test_node.h"

#include <Windows.h>

#include "entity.h"

CubeTestNode::CubeTestNode(EntityID id, Material mat)
    : SceneNode(id, "CubeTestNode", &DirectX::XMMatrixIdentity(), &DirectX::XMMatrixIdentity(), RenderPass::Global, mat)
{
}

CubeTestNode::~CubeTestNode()
{
}

void CubeTestNode::render(Scene* scene)
{
	static float maxX = 1.0f;
	static float minZ = 0.5f;
	static float maxZ = 10.0f;
	static float seconds = 10.0f;
	
	static float x = 0;
	static float y = 0;
	static float u = 0;
	static float l = 0;
	if (GetAsyncKeyState(VK_UP))
	{
		u += 0.01;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		u += -0.01;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		l += -0.01;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		l += 0.01;
	}
	x -= l;
	y += u;
	DirectX::XMMATRIX model = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f) * DirectX::XMMatrixTranslation(x, y, 0.0f);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveLH(maxX, maxX * 640 / 480, minZ, maxZ);
	m_Attributes.getMaterial().m_VSConstantBuffer.m_M = model;
	m_Attributes.getMaterial().m_VSConstantBuffer.m_V = view;
	m_Attributes.getMaterial().m_VSConstantBuffer.m_P = projection;
	m_Attributes.getShader()->setConstantBuffer(m_Attributes.getMaterial().m_VSConstantBuffer);
	SceneNode::render(scene);
}
