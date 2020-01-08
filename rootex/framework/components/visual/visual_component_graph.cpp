#include "visual_component_graph.h"

VisualComponentGraph::VisualComponentGraph(int width, int height)
    : m_Root(new RootVisualComponent())
    , m_Camera(new CameraVisualComponent())
    , m_Renderer(new Renderer(width, height))
{
	m_TransformationStack.push_back(Matrix::Identity);
}

VisualComponentGraph::~VisualComponentGraph()
{
}

void VisualComponentGraph::render()
{
	if (m_Root && m_Camera)
	{
		if (m_Root->preRender(this))
		{
			if (m_Root->isVisible(this))
			{
				m_Root->render(this);
			}
			m_Root->renderChildren(this);
			m_Root->postRender(this);
		}
	}
}

bool VisualComponentGraph::load()
{
	if (!m_Root)
	{
		return true;
	}
	return m_Root->load(this);
}

void VisualComponentGraph::recoverLostDevice()
{
	ERR("Fatal error: D3D Device lost");
}

bool VisualComponentGraph::addChild(VisualComponent* child)
{
	return m_Root->addChild(child);
}

bool VisualComponentGraph::removeChild(Ref<VisualComponent> child)
{
	return m_Root->removeChild(child);
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
