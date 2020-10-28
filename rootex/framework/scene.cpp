#include "scene.h"

#include "ecs_factory.h"
#include "resource_loader.h"

static int CurrentSceneCount = ROOT_SCENE_ID + 1;

void to_json(JSON::json& j, const SceneSettings& s)
{
	j["preloads"] = s.preloads;
	j["camera"] = s.camera;
	j["listener"] = s.listener;
	j["inputSchemes"] = s.inputSchemes;
	j["startScheme"] = s.startScheme;
}

void from_json(const JSON::json& j, SceneSettings& s)
{
	s.preloads = (Vector<String>)j.value("preloads", Vector<String>());
	s.camera = j.value("camera", ROOT_SCENE_ID);
	s.listener = j.value("listener", ROOT_SCENE_ID);
	s.inputSchemes = j.value("inputSchemes", JSON::json::object());
	s.startScheme = j.value("startScheme", String());
}

void Scene::ResetCounter()
{
	CurrentSceneCount = ROOT_SCENE_ID + 1;
}

void Scene::destroyChildren()
{
	m_ChildrenScenes.clear();
}

void Scene::RegisterAPI(sol::table& rootex)
{
	sol::usertype<Scene> scene = rootex.new_usertype<Scene>("Scene");
	scene["CreateEmpty"] = &CreateEmpty;
	scene["CreateEmptyWithEntity"] = &CreateEmptyWithEntity;
	scene["CreateFromFile"] = &CreateFromFile;
	scene["addChild"] = &addChild;
	scene["removeChild"] = &removeChild;
	scene["snatchChild"] = &snatchChild;
	scene["setName"] = &setName;
	scene["setEntity"] = &setEntity;
	scene["getID"] = &getID;
	scene["getParent"] = &getParent;
	scene["getChildren"] = &getChildren;
	scene["getEntity"] = &getEntity;
	scene["getName"] = &getName;
	scene["getFullName"] = &getFullName;
}

Ptr<Scene> Scene::Create(const JSON::json& sceneData)
{
	CurrentSceneCount = CurrentSceneCount > sceneData.value("ID", CurrentSceneCount) ? CurrentSceneCount : sceneData.value("ID", CurrentSceneCount);
	Ptr<Scene> thisScene(std::make_unique<Scene>(CurrentSceneCount, sceneData.value("name", String("Untitled")), sceneData.value("file", String()), sceneData.value("settings", SceneSettings())));
	CurrentSceneCount++;

	if (sceneData.contains("entity"))
	{
		thisScene->m_Entity = ECSFactory::CreateEntity(thisScene.get(), sceneData["entity"]);
	}
	if (sceneData.contains("children"))
	{
		for (auto& childScene : sceneData["children"])
		{
			if (!thisScene->addChild(Create(childScene)))
			{
				WARN("Could not add child scene to " + thisScene->getName() + " scene");
			}
		}
	}
	return thisScene;
}

Ptr<Scene> Scene::CreateFromFile(const String& sceneFile)
{
	if (TextResourceFile* t = ResourceLoader::CreateTextResourceFile(sceneFile))
	{
		if (t->isDirty())
		{
			t->reimport();
		}
		return Create(JSON::json::parse(t->getString()));
	}
	return nullptr;
}

Ptr<Scene> Scene::CreateEmpty()
{
	return Create({});
}

Ptr<Scene> Scene::CreateEmptyAtPath(const String& sceneFile)
{
	return Create({ { "entity", {} }, { "file", sceneFile } });
}

Ptr<Scene> Scene::CreateEmptyWithEntity()
{
	return Create({ { "entity", {} } });
}

Ptr<Scene> Scene::CreateRootScene()
{
	static bool called = false;
	if (called)
	{
		ERR("Denied creating a second root scene. Scene hierarchy may be corrupted.");
		return nullptr;
	}

	Ptr<Scene> root = std::make_unique<Scene>(ROOT_SCENE_ID, "Root", "", SceneSettings());
	root->m_Entity = ECSFactory::CreateRootEntity(root.get());

	called = true;
	return root;
}

Scene* Scene::findScene(SceneID scene)
{
	if (scene == getID())
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

void Scene::reload()
{
	if (m_SceneFile.empty())
	{
		return;
	}

	TextResourceFile* t = ResourceLoader::CreateTextResourceFile(m_SceneFile);
	t->reimport();

	const JSON::json& sceneData = JSON::json::parse(t->getString());
	if (sceneData.contains("entity"))
	{
		setEntity(ECSFactory::CreateEntity(this, sceneData["entity"]));
	}
	else
	{
		setEntity(Ptr<Entity>());
	}

	m_ChildrenScenes.clear();
	if (sceneData.contains("children"))
	{
		for (auto& childScene : sceneData["children"])
		{
			if (!addChild(Create(childScene)))
			{
				WARN("Could not add child scene to " + getName() + " scene");
			}
		}
	}
}

void Scene::onLoad()
{
	if (m_Entity)
	{
		m_Entity->onAllEntitiesAdded();
	}
	for (auto& child : m_ChildrenScenes)
	{
		child->onLoad();
	}
}

bool Scene::snatchChild(Ptr<Scene>& child)
{
	return child->getParent()->removeChild(child.get()) && addChild(child);
}

bool Scene::addChild(Ptr<Scene>& child)
{
	if (!child)
	{
		WARN("Tried to add a null scene to: " + getFullName() + ". Denied.");
		return false;
	}

	auto& findIt = std::find(m_ChildrenScenes.begin(), m_ChildrenScenes.end(), child);
	if (findIt == m_ChildrenScenes.end())
	{
		child->m_ParentScene = this;
		m_ChildrenScenes.emplace_back(std::move(child));
		child.reset();
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

void Scene::setName(const String& name)
{
	m_Name = name;
	m_FullName = name + " # " + std::to_string(m_ID);
}

JSON::json Scene::getJSON() const
{
	JSON::json j;

	j["name"] = m_Name;
	j["file"] = m_SceneFile;
	j["ID"] = m_ID;
	j["entity"] = nullptr;
	if (m_Entity)
	{
		j["entity"] = m_Entity->getJSON();
	}
	j["settings"] = m_Settings;

	j["children"] = JSON::json::array();
	for (auto& child : m_ChildrenScenes)
	{
		j["children"].push_back(child->getJSON());
	}

	return j;
}

Scene::Scene(SceneID id, const String& name, const String& sceneFile, const SceneSettings& settings)
    : m_Name(name)
    , m_ID(id)
    , m_Settings(settings)
    , m_SceneFile(sceneFile)
{
	setName(m_Name);
}

Scene::~Scene()
{
	destroyChildren();
	PRINT("Deleted scene: " + getFullName());
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
#include "scene_loader.h"

void SceneSettings::drawSceneSelectables(Scene* scene, SceneID& toSet)
{
	if (ImGui::Selectable(scene->getFullName().c_str()))
	{
		toSet = scene->getID();
	}
	for (auto& child : scene->getChildren())
	{
		drawSceneSelectables(child.get(), toSet);
	}
}

void SceneSettings::draw()
{
	ImGui::Text("Preloads");
	int i = 0;
	for (auto& preload : preloads)
	{
		ImGui::PushID(i);
		ImGui::InputText("##", &preload);
		ImGui::PopID();
	}
	if (ImGui::Button("+")) 
	{
		preloads.push_back("");
	}
	if (!preloads.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("-"))
		{
			preloads.pop_back();
		}
	}
	if (ImGui::BeginCombo("Camera", SceneLoader::GetSingleton()->getRootScene()->findScene(camera)->getFullName().c_str()))
	{
		drawSceneSelectables(SceneLoader::GetSingleton()->getRootScene(), camera);
		ImGui::EndCombo();
	}
	if (ImGui::BeginCombo("Listener", SceneLoader::GetSingleton()->getRootScene()->findScene(listener)->getFullName().c_str()))
	{
		drawSceneSelectables(SceneLoader::GetSingleton()->getRootScene(), listener);
		ImGui::EndCombo();
	}
	String schemes = inputSchemes.dump(4);
	if (ImGui::InputTextMultiline("Input Schemes", &schemes))
	{
		try
		{
			inputSchemes = JSON::json::parse(schemes);
		}
		catch (std::exception e)
		{
			PRINT(e.what());
		}
	}
	ImGui::InputText("Start Scheme", &startScheme);
}
#endif // ROOTEX_EDITOR
