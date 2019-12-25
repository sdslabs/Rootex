#include "scene.h"

#include "root_node.h"

Scene::Scene(int width, int height)
    : m_Root(new RootNode(nullptr))
    , m_Camera(new CameraNode(DirectX::XMMatrixIdentity()))
    , m_Renderer(new Renderer(width, height))
{
	m_TransformationStack.push_back(DirectX::XMMatrixIdentity());
}

Scene::~Scene()
{
}

void Scene::render()
{
	if (m_Root && m_Camera)
	{
		m_Camera->setSceneView(this);

		if (m_Root->preRender(this))
		{
			m_Root->render(this);
			m_Root->renderChildren(this);
			m_Root->postRender(this);
		}
	}
}

bool Scene::load()
{
	if (!m_Root)
	{
		return true;
	}
	return m_Root->load(this);
}

void Scene::recoverLostDevice()
{
	ERR("Fatal error: D3D Device lost");
}

bool Scene::update(float deltaMilliseconds)
{
	if (!m_Root)
	{
		return true;
	}
	m_Root->update(this, deltaMilliseconds);
}

bool Scene::addChild(EntityID id, Ref<SceneNode> child)
{
	m_SceneEntityNodeMap[id] = child;

	return m_Root->addChild(child);
}

bool Scene::removeChild(EntityID id)
{
	Ref<SceneNode> removed = m_SceneEntityNodeMap[id];
	m_SceneEntityNodeMap.erase(id);
	return m_Root->removeChild(removed);
}

SceneNode* Scene::findNode(EntityID id)
{
	auto& findIt = m_SceneEntityNodeMap.find(id);
	if (findIt != m_SceneEntityNodeMap.end())
	{
		return findIt->second.get();
	}

	return nullptr;
}

void Scene::pushMatrix(const AlignedMatrix& transform)
{
	m_TransformationStack.push_back(DirectX::XMMatrixMultiply(transform, m_TransformationStack.back()));
}

void Scene::popMatrix()
{
	m_TransformationStack.pop_back();
}

void Scene::setCamera(Ref<CameraNode> camera)
{
	m_Camera.reset();
	m_Camera = camera;
}

const DirectX::XMMATRIX* Scene::getTopMatrix() const
{
	return &m_TransformationStack.back();
}
