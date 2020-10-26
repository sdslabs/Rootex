#include "scene.h"

#include "ecs_factory.h"

static int CurrentSceneCount = ROOT_SCENE_ID + 1;

Ref<Scene> Scene::Create(const JSON::json& sceneData)
{
	CurrentSceneCount = CurrentSceneCount > sceneData["ID"] ? CurrentSceneCount : sceneData["ID"];
	Ref<Scene> thisScene(std::make_shared<Scene>(CurrentSceneCount, sceneData.value("name", String("Untitled"))));
	CurrentSceneCount++;

	if (sceneData["entity"] != nullptr)
	{
		thisScene->m_Entity = ECSFactory::CreateEntity(thisScene.get(), sceneData["entity"]);
	}

	for (auto& childScene : sceneData["children"])
	{
		if (!thisScene->addChild(std::make_shared<Scene>(Create(childScene))))
		{
			WARN("Could not add new scene to " + thisScene->getName() + " scene");
		}
	}
	return thisScene;
}

Ref<Scene> Scene::CreateRootScene()
{
	static bool called = false;
	if (called)
	{
		ERR("Denied creating a second root scene. Scene hierarchy may be corrupted.");
		return nullptr;
	}

	Ref<Scene> root = std::make_shared<Scene>(ROOT_SCENE_ID, "Root");
	root->m_Entity = ECSFactory::CreateRootEntity(root.get());

	called = true;
	return root;
}

Scene* Scene::findScene(Scene* scene)
{
	if (scene == this)
	{
		return this;
	}
	for (auto& child : m_ChildrenScenes)
	{
		if (Scene* entityScene = child->findScene(scene))
		{
			return entityScene;
		}
	}
	return nullptr;
}

bool Scene::snatchChild(Ref<Scene>& child)
{
	return child->getParent()->removeChild(child.get()) && addChild(child);
}

bool Scene::addChild(Ref<Scene>& child)
{
	auto& findIt = std::find(m_ChildrenScenes.begin(), m_ChildrenScenes.end(), child);
	if (findIt == m_ChildrenScenes.end())
	{
		child->m_ParentScene = this;
		m_ChildrenScenes.emplace_back(child);
		return true;
	}
	return false;
}

bool Scene::removeChild(Scene* toRemove)
{
	for (auto& child = m_ChildrenScenes.begin(); child != m_ChildrenScenes.end(); child++)
	{
		if ((*child).get() == toRemove)
		{
			m_ChildrenScenes.erase(child);
			return true;
		}
	}
	return false;
}

JSON::json Scene::getJSON() const
{
	JSON::json j;

	j["name"] = m_Name;
	j["ID"] = m_ID;
	j["entity"] = nullptr;
	if (m_Entity)
	{
		j["entity"] = m_Entity->getJSON();
	}

	j["children"] = JSON::json::array();
	for (auto& child : m_ChildrenScenes)
	{
		j["children"].push_back(child->getJSON());
	}

	return j;
}

Scene::Scene(SceneID id, const String& name)
    : m_Name(name)
    , m_ID(id)
{
}

Scene::Scene(SceneID id, const String& name, Ref<Entity>& entity)
    : m_Entity(entity)
    , m_Name(name)
    , m_ID(id)
{
}
