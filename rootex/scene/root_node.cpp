#include "root_node.h"

#include "entity_factory.h"
#include "sky_box.h"

RootNode::RootNode(Entity* entity)
    : SceneNode(INVALID_ID, "Root", Matrix::Identity, nullptr, RenderPass::Global, Material())
{
	m_Children.reserve((size_t)RenderPass::End);

	m_GlobalGroup.reset(new SceneNode(
	    INVALID_ID,
	    "GlobalGroup",
	    Matrix::Identity, nullptr, 
		RenderPass::Global, Material()));
	m_GlobalGroup->setVisibility(false);
	m_Children.push_back(m_GlobalGroup);

	m_StaticGroup.reset(new SceneNode(
	    INVALID_ID,
	    "StaticGroup",
	    Matrix::Identity, nullptr, 
		RenderPass::Static, Material()));
	m_StaticGroup->setVisibility(false);
	m_Children.push_back(m_StaticGroup);

	m_EntityGroup.reset(new SceneNode(
	    INVALID_ID,
	    "EntityGroup",
	    Matrix::Identity, nullptr, 
		RenderPass::Dynamic, Material()));
	m_EntityGroup->setVisibility(false);
	m_Children.push_back(m_EntityGroup);

	m_SkyGroup.reset(new SceneNode(
	    INVALID_ID,
	    "SkyGroup",
	    Matrix::Identity, nullptr, 
		RenderPass::Background, Material()));
	m_SkyGroup->setVisibility(false);
	m_Children.push_back(m_SkyGroup);

	m_EditorGroup.reset(new SceneNode(
	    INVALID_ID,
	    "EditorGroup",
	    Matrix::Identity, nullptr, 
		RenderPass::Editor, Material()));
	m_EditorGroup->setVisibility(false);
	m_Children.push_back(m_EditorGroup);
}

RootNode::~RootNode()
{
}

bool RootNode::addChild(Ref<SceneNode> child)
{
	RenderPass pass = child->getAttributes()->getRenderPass();

	switch (pass)
	{
	case RenderPass::Global:
		m_GlobalGroup->addChild(child);
		return true;
		break;
	case RenderPass::Background:
		m_SkyGroup->addChild(child);
		return true;
		break;
	case RenderPass::Static:
		m_StaticGroup->addChild(child);
		return true;
		break;
	case RenderPass::Dynamic:
		m_EntityGroup->addChild(child);
		return true;
		break;
	case RenderPass::Editor:
		m_EditorGroup->addChild(child);
		return true;
		break;
	case RenderPass::End:
		break;
	default:
		break;
	}
	return false;
}

void RootNode::renderChildren(Scene* scene)
{
	for (int pass = (int)RenderPass::Global; pass <= (int)RenderPass::End; pass++)
	{
		switch (pass)
		{
		case (int)RenderPass::Global:
			m_GlobalGroup->renderChildren(scene);
			break;
		case (int)RenderPass::Static:
			m_StaticGroup->renderChildren(scene);
			break;
		case (int)RenderPass::Dynamic:
			m_EntityGroup->renderChildren(scene);
			break;
		case (int)RenderPass::Background:
		{
			SkyBoxHelper skyBoxHelper;
			m_SkyGroup->renderChildren(scene);
		}
		case (int)RenderPass::Editor:
		default:
			break;
		}
	}
}

bool RootNode::isVisible(Scene* scene) const
{
	return false;
}
