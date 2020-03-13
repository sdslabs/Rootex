#include "render_system.h"

#include "framework/components/visual/visual_component.h"
#include "os/timer.h"

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
	m_Root = HierarchySystem::GetSingleton()->getRootHierarchyComponent().get();
	m_HierarchyGraph = HierarchySystem::GetSingleton()->getHierarchyGraph();
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::recoverLostDevice()
{
	ERR("Fatal error: D3D Device lost");
}

void RenderSystem::render()
{
	if (m_Root->preRender(m_HierarchyGraph))
	{
		m_Root->renderChildren(m_HierarchyGraph);
		m_Root->postRender(m_HierarchyGraph);
	}
}

void RenderSystem::pushMatrix(const Matrix& transform)
{
	m_TransformationStack.push_back(m_TransformationStack.back() * transform);
}

void RenderSystem::popMatrix()
{
	m_TransformationStack.pop_back();
}

void RenderSystem::setCamera(Ref<CameraVisualComponent> camera)
{
	m_Camera.reset();
	m_Camera = camera;
}

const Matrix& RenderSystem::getTopMatrix() const
{
	return m_TransformationStack.back();
}
