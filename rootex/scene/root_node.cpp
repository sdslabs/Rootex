#include "root_node.h"

#include "entity_factory.h"
#include "sky_box.h"

RootNode::RootNode(Entity* entity)
    : SceneNode(INVALID_ID, "Root", &DirectX::XMMatrixIdentity(), &DirectX::XMMatrixIdentity(), RenderPass::Global, Material())
{
	m_Children.reserve((size_t)RenderPass::End);

	Ref<SceneNode> globalGroup(new SceneNode(
	    INVALID_ID,
	    "GlobalGroup",
	    &DirectX::XMMatrixIdentity(),
	    &DirectX::XMMatrixIdentity(), RenderPass::Global, Material()));
	m_Children.push_back(globalGroup);
	m_GlobalGroup = globalGroup;

	Ref<SceneNode> staticGroup(new SceneNode(
	    INVALID_ID,
	    "StaticGroup",
	    &DirectX::XMMatrixIdentity(),
	    &DirectX::XMMatrixIdentity(), RenderPass::Static, Material()));
	m_Children.push_back(staticGroup);
	m_StaticGroup = staticGroup;

	Ref<SceneNode> entityGroup(new SceneNode(
	    INVALID_ID,
	    "EntityGroup",
	    &DirectX::XMMatrixIdentity(),
	    &DirectX::XMMatrixIdentity(), RenderPass::Dynamic, Material()));
	m_Children.push_back(entityGroup);
	m_EntityGroup = entityGroup;

	Ref<SceneNode> skyGroup(new SceneNode(
	    INVALID_ID,
	    "SkyGroup",
	    &DirectX::XMMatrixIdentity(),
	    &DirectX::XMMatrixIdentity(), RenderPass::Background, Material()));
	m_Children.push_back(skyGroup);
	m_SkyGroup = skyGroup;

	Ref<SceneNode> editorGroup(new SceneNode(
	    INVALID_ID,
	    "EditorGroup",
	    &DirectX::XMMatrixIdentity(),
	    &DirectX::XMMatrixIdentity(), RenderPass::Editor, Material()));
	m_Children.push_back(editorGroup);
	m_EditorGroup = editorGroup;
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
