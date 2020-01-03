#include "render_system.h"

#include "framework/components/visual/visual_component.h"

RenderSystem* RenderSystem::GetSingleton()
{
	static RenderSystem singleton;
	return &singleton;
}

void RenderSystem::update(float deltaMilliseconds)
{
	ERR("RenderSystem will not update based on time");
}

void RenderSystem::render(VisualComponentGraph* graph, Window* window)
{
	graph->getRenderer()->clear();
	graph->render();
	window->swapBuffers();
}
