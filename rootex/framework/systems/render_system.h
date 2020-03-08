#pragma once

#include "framework/system.h"
#include "framework/components/visual/visual_component_graph.h"
#include "main/window.h"

class RenderSystem : public System
{
	VisualComponentGraph m_VisualGraph;

	void addToVisualGraph(VisualComponent* vc);

	friend class VisualComponent;

public:
	static RenderSystem* GetSingleton();
	
	void render();
};
