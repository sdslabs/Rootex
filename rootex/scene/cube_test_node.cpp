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
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveLH(maxX, maxX * 640.0f / 480.0f, minZ, maxZ);
		
	m_Attributes.getShader()->setConstantBuffer(Shader::ConstantBufferType::Model, model);
	m_Attributes.getShader()->setConstantBuffer(Shader::ConstantBufferType::View, view);
	m_Attributes.getShader()->setConstantBuffer(Shader::ConstantBufferType::Projection, projection);
	
	PSConstantBuffer pcb;
	pcb.m_Colors[0] = { 0.0f, 0.0f, 1.0f, 1.0f };
	pcb.m_Colors[1] = { 0.0f, 1.0f, 1.0f, 1.0f };
	pcb.m_Colors[2] = { 0.0f, 0.0f, 1.0f, 1.0f };
	pcb.m_Colors[3] = { 1.0f, 0.0f, 0.0f, 1.0f };
	pcb.m_Colors[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	pcb.m_Colors[5] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_Attributes.getShader()->setConstantBuffer(pcb);

	SceneNode::render(scene);
}
