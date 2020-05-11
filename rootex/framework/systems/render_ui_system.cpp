#include "render_ui_system.h"

#include "renderer/rendering_device.h"

#include "components/visual/ui_component.h"

RenderUISystem::RenderUISystem()
{
	m_UITransformationStack.push_back(Matrix::Identity);
}

RenderUISystem* RenderUISystem::GetSingleton()
{
	static RenderUISystem singleton;
	return &singleton;
}

void RenderUISystem::render()
{
	RenderingDevice::GetSingleton()->beginDrawUI();
	UIComponent* ui = nullptr;
	for (auto& component : s_Components[UIComponent::s_ID])
	{
		ui = (UIComponent*)component;
		if (ui->isVisible())
		{
			ui->preRender();
			ui->render();
			ui->postRender();
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
