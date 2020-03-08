#include "render_system.h"

#include "framework/components/visual/visual_component.h"
#include "os/timer.h"

RenderSystem* RenderSystem::GetSingleton()
{
	static RenderSystem singleton;
	return &singleton;
}

void RenderSystem::addToVisualGraph(VisualComponent* vc)
{
	m_VisualGraph.addChild(vc->getOwner());
}

void RenderSystem::render()
{
	m_VisualGraph.render();
}
