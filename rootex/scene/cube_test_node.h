#pragma once

#include "scene.h"
#include "scene_node.h"

class CubeTestNode : public SceneNode
{
	PSConstantBuffer m_PCB;

public:
	CubeTestNode(EntityID id, Material mat);
	~CubeTestNode();

	void render(Scene* scene) override;
};
