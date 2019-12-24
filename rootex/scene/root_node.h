#pragma once

#include "scene_node.h"

class RootNode : public SceneNode
{
	Ref<SceneNode> m_StaticGroup;
	Ref<SceneNode> m_EntityGroup;
	Ref<SceneNode> m_SkyGroup;
	Ref<SceneNode> m_EditorGroup;

public:
	RootNode(Entity* entity);
	~RootNode();

	virtual bool addChild(Ref<SceneNode> child) override;
	virtual void render(Scene* scene) override;
	virtual bool isVisible(Scene* scene) const;
};
