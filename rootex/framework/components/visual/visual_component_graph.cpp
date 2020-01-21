#include "visual_component_graph.h"

#include "resource_loader.h"

VisualComponentGraph::VisualComponentGraph(int width, int height)
    : m_Camera(new CameraVisualComponent())
    , m_Renderer(new Renderer(width, height))
{
	m_Root = EntityFactory::GetSingleton()->createEmptyEntity();
	m_RootHierarchyComponent.reset(new RootHierarchyComponent());
	m_Root->addComponent(m_RootHierarchyComponent);
	m_RootHierarchyComponent->setOwner(m_Root);

	m_TransformationStack.push_back(Matrix::Identity);
}

VisualComponentGraph::~VisualComponentGraph()
{
}

void VisualComponentGraph::render()
{
	if (m_RootHierarchyComponent->preRender(this))
	{
		m_RootHierarchyComponent->renderChildren(this);
		m_RootHierarchyComponent->postRender(this);
	}
}

void VisualComponentGraph::recoverLostDevice()
{
	ERR("Fatal error: D3D Device lost");
}

bool VisualComponentGraph::addChild(Ref<Entity> child)
{
	return m_RootHierarchyComponent->addChild(child);
}

bool VisualComponentGraph::removeChild(Ref<Entity> child)
{
	return m_RootHierarchyComponent->removeChild(child);
}

void VisualComponentGraph::pushMatrix(const Matrix& transform)
{
	m_TransformationStack.push_back(transform * m_TransformationStack.back());
}

void VisualComponentGraph::popMatrix()
{
	m_TransformationStack.pop_back();
}

void VisualComponentGraph::setCamera(Ref<CameraVisualComponent> camera)
{
	m_Camera.reset();
	m_Camera = camera;
}

const Matrix& VisualComponentGraph::getTopMatrix() const
{
	return m_TransformationStack.back();
}
