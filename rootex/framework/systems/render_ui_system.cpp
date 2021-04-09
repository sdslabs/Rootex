#include "render_ui_system.h"

#include "renderer/rendering_device.h"
#include "framework/ecs_factory.h"
#include "components/visual/ui/text_ui_component.h"

RenderUISystem::RenderUISystem()
    : System("RenderUISystem", UpdateOrder::RenderUI, true)
{
	m_UITransformationStack.push_back(Matrix::Identity);
}

RenderUISystem* RenderUISystem::GetSingleton()
{
	static RenderUISystem singleton;
	return &singleton;
}

void RenderUISystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	RenderingDevice::GetSingleton()->beginDrawUI();
	for (auto& tui : ECSFactory::GetAllTextUIComponent())
	{
		if (tui.isVisible())
		{
			tui.preRender();
			tui.render();
			tui.postRender();
		}
	}
	RenderingDevice::GetSingleton()->endDrawUI();
}

void RenderUISystem::pushUIMatrix(const Matrix& transform)
{
	m_UITransformationStack.push_back(transform * m_UITransformationStack.back());
}

void RenderUISystem::popUIMatrix()
{
	m_UITransformationStack.pop_back();
}

Matrix RenderUISystem::getTopUIMatrix()
{
	return m_UITransformationStack.back();
}
