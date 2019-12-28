#include "cube_test_node.h"

#include <Windows.h>

#include "entity.h"

CubeTestNode::CubeTestNode(EntityID id, Material mat)
    : SceneNode(id, "CubeTestNode", Matrix::Identity, nullptr, RenderPass::Global, mat)
{
	m_PCB.m_Colors[0] = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_PCB.m_Colors[1] = { 0.0f, 1.0f, 1.0f, 1.0f };
	m_PCB.m_Colors[2] = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_PCB.m_Colors[3] = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_PCB.m_Colors[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	m_PCB.m_Colors[5] = { 1.0f, 0.0f, 1.0f, 1.0f };
}

CubeTestNode::~CubeTestNode()
{
}

void CubeTestNode::render(Scene* scene)
{	
	m_Attributes.getShader()->setConstantBuffer(m_PCB);

	SceneNode::render(scene);
}
