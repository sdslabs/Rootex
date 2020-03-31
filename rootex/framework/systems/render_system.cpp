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
		child->getOwner()->getComponent<TransformComponent>()->m_TransformBuffer.m_AbsoluteTransform = getTopMatrix();
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
	Ref<HierarchyComponent> rootHC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<HierarchyComponent>();
	calculateTransforms(rootHC.get());

	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderingDevice::GetSingleton()->setRasterizerState();
	RenderingDevice::GetSingleton()->setDepthStencilState();

	Ref<VisualComponent> rootVC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<VisualComponent>();
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

void RenderSystem::pushMatrix(const Matrix& transform)
{
	m_TransformationStack.push_back(transform * m_TransformationStack.back());
}

void RenderSystem::pushMatrixOverride(const Matrix& transform)
{
	m_TransformationStack.push_back(transform);
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
