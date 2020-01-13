#include "root_visual_component.h"

#include "entity_factory.h"
#include "resource_loader.h"
#include "sky_box_helper.h"

RootVisualHierarchyComponent::RootVisualHierarchyComponent()
    : VisualComponent(RenderPass::Global, nullptr, nullptr)
{
	m_Children.reserve((size_t)RenderPass::End);

	//m_GlobalGroup.reset(new VisualComponent(
	//    RenderPass::Global,
	//	nullptr,
	//	nullptr));
	//m_GlobalGroup->setVisibility(false);
	//m_Children.push_back(m_GlobalGroup);

	//m_StaticGroup.reset(new VisualComponent(
	//    RenderPass::Static,
	//	nullptr,
	//	nullptr));
	//m_StaticGroup->setVisibility(false);
	//m_Children.push_back(m_StaticGroup);

	//m_EntityGroup.reset(new VisualComponent(
	//    RenderPass::Dynamic,
	//	nullptr,
	//	nullptr));
	//m_EntityGroup->setVisibility(false);
	//m_Children.push_back(m_EntityGroup);

	//m_SkyGroup.reset(new VisualComponent(
	//    RenderPass::Background,
	//	nullptr,
	//    nullptr));
	//m_SkyGroup->setVisibility(false);
	//m_Children.push_back(m_SkyGroup);

	//m_EditorGroup.reset(new VisualComponent(
	//    RenderPass::Editor,
	//	nullptr,
	//    nullptr));
	//m_EditorGroup->setVisibility(false);
	//m_Children.push_back(m_EditorGroup);
}

RootVisualHierarchyComponent::~RootVisualHierarchyComponent()
{
}

bool RootVisualHierarchyComponent::preRender(VisualComponentGraph* visualComponentGraph)
{
	return true;
}

bool RootVisualHierarchyComponent::addChild(Ref<Entity> child)
{
	//TODO- FIX THIS
	RenderPass pass = child->getComponent<DiffuseVisualComponent>()->getAttributes()->getRenderPass();

	switch (pass)
	{
	case RenderPass::Global:
		m_GlobalGroup.addChild(child);
		return true;
		break;
	case RenderPass::Background:
		m_SkyGroup.addChild(child);
		return true;
		break;
	case RenderPass::Static:
		m_StaticGroup.addChild(child);
		return true;
		break;
	case RenderPass::Dynamic:
		m_EntityGroup.addChild(child);
		return true;
		break;
	case RenderPass::Editor:
		m_EditorGroup.addChild(child);
		return true;
		break;
	case RenderPass::End:
		break;
	default:
		break;
	}
	return false;
}

void RootVisualHierarchyComponent::renderChildren(VisualComponentGraph* scene)
{
	for (int pass = (int)RenderPass::Global; pass <= (int)RenderPass::End; pass++)
	{
		switch (pass)
		{
		case (int)RenderPass::Global:
			//m_GlobalGroup->renderChildren(scene);
			for (auto& x : m_GlobalGroup.m_Children)
			{
				//TODO- FIX THIS
				x->getComponent<DiffuseVisualComponent>()->renderChildren(scene);

				VisualComponent* childVisualComponent = x->getComponent<DiffuseVisualComponent>();

				childVisualComponent->preRender(scene);

				if (childVisualComponent->isVisible(scene))
				{
					// Assumed to be opaque
					childVisualComponent->render(scene);
				}
				childVisualComponent->renderChildren(scene);

				childVisualComponent->postRender(scene);
			}
			break;
		case (int)RenderPass::Static:
			//m_StaticGroup->renderChildren(scene);
			for (auto& x : m_StaticGroup.m_Children)
				x->getComponent<VisualComponent>()->renderChildren(scene);
			break;
		case (int)RenderPass::Dynamic:
			//m_EntityGroup->renderChildren(scene);
			for (auto& x : m_EntityGroup.m_Children)
				x->getComponent<VisualComponent>()->renderChildren(scene);
			break;
		case (int)RenderPass::Background:
		{
			SkyBoxHelper skyBoxHelper;
			//m_SkyGroup->renderChildren(scene);
			for (auto& x : m_SkyGroup.m_Children)
				x->getComponent<VisualComponent>()->renderChildren(scene);
		}
		case (int)RenderPass::Editor:
		default:
			break;
		}
	}
}

bool RootVisualHierarchyComponent::isVisible(VisualComponentGraph* scene) const
{
	return false;
}

void RootVisualHierarchyComponent::postRender(VisualComponentGraph* visualComponentGraph)
{
}
