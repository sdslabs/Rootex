#include "scene.h"

#include "ecs_factory.h"
#include "resource_loader.h"
#include "scene_loader.h"

static int NextSceneID = ROOT_SCENE_ID + 1;
Vector<Scene*> Scene::s_Scenes;

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
	if (j["inputSchemes"].is_null())
	{
		s.inputSchemes = {};
	}
	else
	{
		s.inputSchemes = j.value("inputSchemes", HashMap<String, InputScheme>());
	}
	s.startScheme = j.value("startScheme", String());
}

void Scene::ResetNextID()
{
	NextSceneID = ROOT_SCENE_ID + 1;
}

Ptr<Scene> Scene::Create(const JSON::json& sceneData)
{
	SceneID thisSceneID = NextSceneID;
	if (sceneData.contains("ID")) 
	{
		thisSceneID = sceneData["ID"];
		if (thisSceneID >= NextSceneID)
		{
			NextSceneID = thisSceneID + 1;
		}
	}
	else
	{
		NextSceneID++;
	}
	
	Ptr<Scene> thisScene(std::make_unique<Scene>(thisSceneID, sceneData.value("name", String("Untitled")), sceneData.value("file", String()), sceneData.value("settings", SceneSettings())));
	s_Scenes.push_back(thisScene.get());
	
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
	return Create(JSON::json::object());
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
	s_Scenes.push_back(root.get());

	called = true;
	return root;
}

Vector<Scene*> Scene::FindScenesByName(const String& name) 
{ 
	Vector<Scene*> foundScenes;
	for (auto& scene : s_Scenes)
	{
		if (scene->m_Name == name)
		{
			foundScenes.push_back(scene);
		}
	}
	return foundScenes;
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

bool Scene::snatchChild(Scene* child)
{
	if (!checkCycle(child))
	{
		return false;
	}
	
	Vector<Ptr<Scene>>& children = child->getParent()->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		if (children.at(i).get() == child)
		{
			m_ChildrenScenes.push_back(std::move(children[i]));
			children.erase(children.begin() + i);
		}
	}
	child->m_ParentScene = this;
	return true;
}

bool Scene::checkCycle(Scene* child)
{
	if (child->findScene(this->getID()) != nullptr)
	{
		WARN("Tried to make a scene its own child's child");
		return false;
	}
	return true;
}

bool Scene::addChild(Ptr<Scene>& child)
{
	if (!child)
	{
		WARN("Tried to add a null scene to: " + getFullName() + ". Denied.");
		return false;
	}
	if (!checkCycle(child.get()))
	{
		return false;
	}
	auto& findIt = std::find(m_ChildrenScenes.begin(), m_ChildrenScenes.end(), child);
	if (findIt == m_ChildrenScenes.end())
	{
		child->m_ParentScene = this;
		m_ChildrenScenes.emplace_back(std::move(child));
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
	int index;
	for (int i = 0; i < s_Scenes.size(); i++)
	{
		if (s_Scenes[i] == this)
		{
			index = i;
		}
	}
	s_Scenes.erase(s_Scenes.begin() + index);
	m_ChildrenScenes.clear();
	PRINT("Deleted scene: " + getFullName());
}


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

	if (ImGui::BeginCombo("Start Scheme", startScheme.c_str()))
	{
		for (auto& [name, inputScheme] : inputSchemes)
		{
			if (ImGui::MenuItem(name.c_str()))
			{
				startScheme = name;
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Text("Input Schemes");
	i = 0;
	const String* inputSchemeToRemove = nullptr;
	for (auto& [name, inputScheme] : inputSchemes)
	{
		ImGui::PushID(i);

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 100);
		if (ImGui::ListBoxHeader(name.c_str()))
		{
			int boolDeletion = -1;
			for (int p = 0 ; p < inputScheme.bools.size(); p++)
			{
				InputDescription& boolInput = inputScheme.bools[p];
				ImGui::PushID(i);
					
				if (ImGui::SmallButton("x"))
				{
					boolDeletion = p;
				}
				ImGui::SameLine();

				drawInputScheme(boolInput);

				ImGui::PopID();
				i++;
			}
			int floatDeletion = -1;
			for (int p = 0; p < inputScheme.floats.size(); p++)
			{
				InputDescription& floatInput = inputScheme.floats[p];
				ImGui::PushID(i);

				if (ImGui::SmallButton("x"))
				{
					floatDeletion = p;
				}
				ImGui::SameLine();

				drawInputScheme(floatInput);

				ImGui::PopID();
				i++;
			}

			if (boolDeletion != -1)
			{
				inputScheme.bools.erase(inputScheme.bools.begin() + boolDeletion);
			}
			if (floatDeletion != -1)
			{
				inputScheme.floats.erase(inputScheme.floats.begin() + floatDeletion);
			}

			ImGui::ListBoxFooter();
		}
		if (ImGui::Button("Remove Scheme"))
		{
			inputSchemeToRemove = &name;
		}

		static int type = 0;
		ImGui::Combo("Type", &type, "Bool\0Float\0");
		ImGui::SameLine();
		if (ImGui::Button("Add Input"))
		{
			InputDescription inputDesc;
			inputDesc.device = Device::Mouse;
			inputDesc.button = MouseButton::MouseButtonLeft;
			inputDesc.inputEvent = "GameBoolEvent";

			if (type == 0)
			{
				inputScheme.bools.push_back(inputDesc);
			}
			else if (type == 1)
			{
				inputScheme.floats.push_back(inputDesc);
			}
		}

		ImGui::PopID();
		i++;
	}
	
	if (inputSchemeToRemove)
	{
		inputSchemes.erase(*inputSchemeToRemove);
	}

	ImGui::Separator();
	
	static String newSchemeName = "New Scheme";
	ImGui::InputText("##New Scheme", &newSchemeName);
	ImGui::SameLine();
	if (ImGui::Button("Add Scheme"))
	{
		inputSchemes.insert({ newSchemeName, InputScheme() });
	}
}

void SceneSettings::drawInputScheme(InputDescription& inputDesc)
{
	int device = (int)inputDesc.device;
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() / 3.0f);
	if (ImGui::Combo("##device", &device, "Mouse\0Keyboard\0Pad1\0Pad2\0"))
	{
		inputDesc.device = (Device)device;
	}
	ImGui::SameLine();
	int button = inputDesc.button;
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() / 3.0f);
	if (ImGui::InputInt("##button", &button))
	{
		inputDesc.button = (DeviceButtonID)button;
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() / 3.0f);
	ImGui::InputTextWithHint("##event", "Event", &inputDesc.inputEvent);
}
