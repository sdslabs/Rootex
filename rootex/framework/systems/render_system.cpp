#include "render_system.h"

#include "framework/components/visual/sky_box_helper.h"
#include "framework/components/visual/visual_component.h"

RenderSystem* RenderSystem::GetSingleton()
{
	static RenderSystem singleton;
	return &singleton;
}

RenderSystem::RenderSystem()
    : m_Renderer(new Renderer())
    , m_Camera(new CameraVisualComponent())
{
	m_TransformationStack.push_back(Matrix::Identity);
	m_UITransformationStack.push_back(Matrix::Identity);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::calculateTransforms(HierarchyComponent* hierarchyComponent)
{
	pushMatrix(hierarchyComponent->getOwner()->getComponent<TransformComponent>()->getLocalTransform());

	for (auto&& child : hierarchyComponent->getChildren())
	{
		child->getOwner()->getComponent<TransformComponent>()->getRenderingBuffer()->m_AbsoluteTransform = getTopMatrix();
		calculateTransforms(child);
	}

	popMatrix();
}

void RenderSystem::renderPassRender(VisualComponent* vc, const RenderPass& renderPass)
{
	vc->preRender();
	if (vc->isVisible())
	{
		vc->render();
	}
	vc->renderChildren(renderPass);
	vc->postRender();
}

void RenderSystem::recoverLostDevice()
{
	ERR("Fatal error: D3D Device lost");
}

void RenderSystem::render()
{
	//Thing to consider here-
	//lets say that physics completes before this even get called, if this happens one time, it isnt an issue
	//but if we fall in a weird condition where physics always seems to complete before render() is called
	//in that case it might be possible that render() never does any rendering
	//I am 99.99% sure that this won't ever happen as render() is being constantly called in an infinite while() but has to also run the logic
	//still for that 0.01% chance I left these comments here
	if (m_physicsCompleteOnce )
	{
		Ref<HierarchyComponent> rootHC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<HierarchyComponent>();
		calculateTransforms(rootHC.get());

		Ref<VisualComponent> rootVC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<VisualComponent>();

		RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderingDevice::GetSingleton()->setRasterizerState();
		RenderingDevice::GetSingleton()->setDepthStencilState();

#ifdef ROOTEX_EDITOR
		renderPassRender(rootVC.get(), RenderPassEditor);
#endif // ROOTEX_EDITOR
		renderPassRender(rootVC.get(), RenderPassMain);
		{
			SkyBoxHelper skyHelper;
			renderPassRender(rootVC.get(), RenderPassSky);
		}
		{
			RenderingDevice::GetSingleton()->beginDrawUI();
			renderPassRender(rootVC.get(), RenderPassUI);
			RenderingDevice::GetSingleton()->endDrawUI();
		}
	}
	PhysicsSystem::GetSingleton()->m_renderCompleteOnce = true;
}

void RenderSystem::pushMatrix(const Matrix& transform)
{
	m_TransformationStack.push_back(transform * m_TransformationStack.back());
}

void RenderSystem::popMatrix()
{
	m_TransformationStack.pop_back();
}

void RenderSystem::pushUIMatrix(const Matrix& transform)
{
	m_UITransformationStack.push_back(transform * m_TransformationStack.back());
}

void RenderSystem::popUIMatrix()
{
	m_UITransformationStack.pop_back();
}

void RenderSystem::setCamera(Ref<CameraVisualComponent> camera)
{
	m_Camera = camera;
}

const Matrix& RenderSystem::getTopMatrix() const
{
	return m_TransformationStack.back();
}

Matrix& RenderSystem::getTopUIMatrix()
{
	return m_UITransformationStack.back();
}
