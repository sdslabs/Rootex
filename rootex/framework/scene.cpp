#include "scene.h"

#include "ecs_factory.h"
#include "resource_loader.h"
#include "scene_loader.h"
#include "systems/script_system.h"
#include "components/visual/camera_component.h"
#include "components/audio/audio_listener_component.h"

Vector<Scene*> Scene::s_Scenes;
SceneID Scene::BaseID;
static SceneID NextSceneID = ROOT_SCENE_ID + 1;

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
	s.preloads = j.value("preloads", ResourceCollection());
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

void to_json(JSON::json& j, const Scene::ImportStyle& s)
{
	j = (int)s;
}

void from_json(const JSON::json& j, Scene::ImportStyle& s)
{
	s = (Scene::ImportStyle)(int)j;
}

void Scene::ResetNextID()
{
	NextSceneID = ROOT_SCENE_ID + 1;
}

void Scene::SetBaseID(const SceneID& inputBaseID)
{
	BaseID = inputBaseID;
	SceneID SceneIDOffset = 4;
	NextSceneID = std::max(std::max(BaseID, NextSceneID), SceneIDOffset);
}

Ptr<Scene> Scene::Create(const JSON::json& sceneData, const bool assignNewIDs)
{
	// Decide ID
	SceneID thisSceneID;
	if (sceneData.contains("ID"))
	{
		NextSceneID = std::max(NextSceneID, (SceneID)sceneData["ID"]);
		if (!assignNewIDs)
		{
			thisSceneID = sceneData["ID"];
		}
		else
		{
			thisSceneID = NextSceneID;
		}
	}
	else
	{
		thisSceneID = NextSceneID;
	}
	NextSceneID++;

	// Decide how to import
	if (sceneData.contains("importStyle") && sceneData["importStyle"] != ImportStyle::Local && sceneData.value("sceneFile", "") == "")
	{
		ERR("Found empty scene file path for an externally imported scene");
	}

	Ptr<Scene> thisScene(std::make_unique<Scene>(
	    thisSceneID,
	    sceneData.value("name", String("Untitled")),
	    sceneData.value("settings", SceneSettings()),
	    sceneData.value("importStyle", ImportStyle::Local),
	    sceneData.value("sceneFile", "")));

	// Make entity and children scenes
	if (sceneData.contains("entity"))
	{
		ECSFactory::FillEntity(thisScene->m_Entity, sceneData["entity"]);
	}
	if (sceneData.contains("children"))
	{
		for (auto& childScene : sceneData["children"])
		{
			if (!thisScene->addChild(Create(childScene, assignNewIDs)))
			{
				WARN("Could not add child scene to " + thisScene->getName() + " scene");
			}
		}
	}
	thisScene->m_IsScenePaused = sceneData.value("pause", true);
	return thisScene;
}

Ptr<Scene> Scene::CreateFromFile(const String& sceneFile)
{
	if (Ref<TextResourceFile> t = ResourceLoader::CreateTextResourceFile(sceneFile))
	{
		if (t->isDirty())
		{
			t->reimport();
		}
		JSON::json importedScene = JSON::json::parse(t->getString());
		importedScene["importStyle"] = ImportStyle::External;
		importedScene["sceneFile"] = sceneFile;
		return Create(importedScene, true);
	}
	return nullptr;
}

Ptr<Scene> Scene::CreateEmpty()
{
	return Create(JSON::json::object(), false);
}

Ptr<Scene> Scene::CreateEmptyAtPath(const String& sceneFile)
{
	return Create({ { "entity", {} }, { "sceneFile", sceneFile } }, false);
}

Ptr<Scene> Scene::CreateRootScene()
{
	static bool called = false;
	if (called)
	{
		ERR("Denied creating a second root scene. Scene hierarchy may be corrupted.");
		return nullptr;
	}

	Ptr<Scene> root = std::make_unique<Scene>(ROOT_SCENE_ID, "Root", SceneSettings(), ImportStyle::Local, "");

	ECSFactory::FillRootEntity(root->getEntity());

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

Scene* Scene::FindSceneByID(const SceneID& id)
{
	for (auto& scene : s_Scenes)
	{
		if (scene->m_ID == id)
		{
			return scene;
		}
	}
	return nullptr;
}

const Vector<Scene*>& Scene::FindAllScenes()
{
	return s_Scenes;
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

void Scene::reimport()
{
	if (m_ImportStyle != ImportStyle::External)
	{
		WARN("Did not reimport local scene. Needs to be external to be reimported.");
		return;
	}

	Ref<TextResourceFile> t = ResourceLoader::CreateTextResourceFile(m_SceneFile);
	t->reimport();

	const JSON::json& sceneData = JSON::json::parse(t->getString());
	m_Entity.clear();
	if (sceneData.contains("entity"))
	{
		ECSFactory::FillEntity(m_Entity, sceneData["entity"]);
	}

	m_ChildrenScenes.clear();
	if (sceneData.contains("children"))
	{
		for (auto& childScene : sceneData["children"])
		{
			if (!addChild(Create(childScene, false)))
			{
				WARN("Could not add child scene to " + getName() + " scene");
			}
		}
	}
}

void Scene::onLoad()
{
	m_Entity.onAllEntitiesAdded();
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
	if (child->findScene(m_ID) != nullptr)
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
		if (getID() != ROOT_SCENE_ID && getName() != "EditorCamera" && getName() != "EditorGrid")
		{
			for (auto&& inputScheme : child->getSettings().inputSchemes)
			{
				m_Settings.inputSchemes.insert(inputScheme);
			}
		}
		m_ChildrenScenes.emplace_back(std::move(child));
		ScriptSystem::GetSingleton()->addEnterScriptEntity(&m_ChildrenScenes.back()->getEntity());
	}
	else
	{
		ERR("Tried to add a duplicate child " + child->getFullName() + " to " + getFullName());
		return false;
	}
	return true;
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

	j["ID"] = m_ID;
	j["name"] = m_Name;
	j["BaseID"] = m_BaseID;
	j["importStyle"] = m_ImportStyle;
	j["sceneFile"] = m_SceneFile;
	j["entity"] = m_Entity.getJSON();
	j["settings"] = m_Settings;
	j["pause"] = m_IsScenePaused;

	j["children"] = JSON::json::array();
	for (auto& child : m_ChildrenScenes)
	{
		j["children"].push_back(child->getJSON());
	}

	return j;
}

Scene::Scene(SceneID id, const String& name, const SceneSettings& settings, ImportStyle importStyle, const String& sceneFile)
    : m_Name(name)
    , m_ID(id)
    , m_Settings(settings)
    , m_ImportStyle(importStyle)
    , m_SceneFile(sceneFile)
    , m_Entity(this)
{
	setName(m_Name);
	setBaseID();
	s_Scenes.push_back(this);
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

void SceneSettings::drawCameraSceneSelectables(Scene* scene, SceneID& toSet)
{
	if (scene->getEntity().getComponent<CameraComponent>())
	{
		if (ImGui::Selectable(scene->getFullName().c_str()))
		{
			toSet = scene->getID();
		}
	}
	for (auto& child : scene->getChildren())
	{
		drawCameraSceneSelectables(child.get(), toSet);
	}
}
void SceneSettings::drawListenerSceneSelectables(Scene* scene, SceneID& toSet)
{
	if (scene->getEntity().getComponent<AudioListenerComponent>())
	{
		if (ImGui::Selectable(scene->getFullName().c_str()))
		{
			toSet = scene->getID();
		}
	}
	for (auto& child : scene->getChildren())
	{
		drawListenerSceneSelectables(child.get(), toSet);
	}
}

void SceneSettings::draw()
{
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 2.0f / 3.0f);
	if (ImGui::ListBoxHeader("Preloads"))
	{
		int toRemove = -1;
		for (int p = 0; p < preloads.size(); p++)
		{
			ImGui::PushID(p);

			if (ImGui::SmallButton("-"))
			{
				toRemove = p;
			}
			ImGui::SameLine();
			ImGui::LabelText(ResourceFile::s_TypeNames.at((ResourceFile::Type)preloads[p].first).c_str(), "%s", preloads[p].second.c_str());

			ImGui::PopID();
		}

		if (toRemove >= 0)
		{
			preloads.erase(preloads.begin() + toRemove);
		}

		ImGui::ListBoxFooter();
	}
	if (ImGui::Button("Add Preload"))
	{
		ImGui::OpenPopup("Preloads Selection");
	}

	if (ImGui::BeginPopup("Preloads Selection"))
	{
		int i = 0;
		for (auto& [resType, resFiles] : ResourceLoader::GetResources())
		{
			for (auto& resFile : resFiles)
			{
				Ref<ResourceFile> res = resFile.lock();
				if (!res)
				{
					continue;
				}

				ImGui::PushID(i++);

				String path = res->getPath().generic_string();

				auto& findIt = std::find(preloads.begin(), preloads.end(), Pair<ResourceFile::Type, String>(resType, path));

				bool enabled = findIt != preloads.end();
				if (ImGui::Checkbox(path.c_str(), &enabled))
				{
					if (enabled)
					{
						preloads.push_back({ resType, path });
					}
					else
					{
						preloads.erase(findIt);
					}
				}
				ImGui::SameLine();
				ImGui::BulletText("%s", ResourceFile::s_TypeNames.at(resType).c_str());

				ImGui::PopID();
			}
		}

		ImGui::EndPopup();
	}

	Scene* cameraScene = SceneLoader::GetSingleton()->getRootScene()->findScene(camera);
	if (!cameraScene)
	{
		cameraScene = SceneLoader::GetSingleton()->getRootScene();
	}
	if (ImGui::BeginCombo("Camera", cameraScene->getFullName().c_str()))
	{
		drawCameraSceneSelectables(SceneLoader::GetSingleton()->getRootScene(), camera);
		ImGui::EndCombo();
	}

	Scene* listenerScene = SceneLoader::GetSingleton()->getRootScene()->findScene(listener);
	if (!listenerScene)
	{
		listenerScene = SceneLoader::GetSingleton()->getRootScene();
	}
	if (ImGui::BeginCombo("Listener", listenerScene->getFullName().c_str()))
	{
		drawListenerSceneSelectables(SceneLoader::GetSingleton()->getRootScene(), listener);
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

	ImGui::Text("Input Schemes:");
	ImGui::Separator();
	int i = 0;
	const String* inputSchemeToRemove = nullptr;
	for (auto& [name, inputScheme] : inputSchemes)
	{
		ImGui::PushID(i);
		ImGui::Text(name.c_str());
		ImGui::Checkbox("Active", &inputScheme.isActive);
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 100);
		if (ImGui::ListBoxHeader(name.c_str()))
		{
			int boolDeletion = -1;
			for (int p = 0; p < inputScheme.bools.size(); p++)
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

		static int type = 0;
		ImGui::Combo("Type", &type, "Bool\0Float\0");
		ImGui::SameLine();
		if (ImGui::Button("Add Input"))
		{
			InputDescription inputDesc;
			inputDesc.device = Device::Mouse;
			inputDesc.button = MouseButton::MouseButtonLeft;
			inputDesc.inputEvent = name + "::GameBoolEvent";

			if (type == 0)
			{
				inputScheme.bools.push_back(inputDesc);
			}
			else if (type == 1)
			{
				inputScheme.floats.push_back(inputDesc);
			}
		}
		if (ImGui::Button("Remove Scheme"))
		{
			inputSchemeToRemove = &name;
		}
		ImGui::PopID();
		ImGui::Separator();
		i++;
	}

	if (inputSchemeToRemove)
	{
		inputSchemes.erase(*inputSchemeToRemove);
		startScheme = "";
	}

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
	Device originalDevice = inputDesc.device;
	int device = (int)originalDevice;
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() / 3.0f);
	if (ImGui::Combo("##device", &device, "Mouse\0Keyboard\0Pad1\0Pad2\0"))
	{
		inputDesc.device = (Device)device;
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() / 3.0f);
	if (inputDesc.device == Device::Mouse)
	{
		inputDesc.button = originalDevice == inputDesc.device ? inputDesc.button : 0;
		auto mouseKeys = InputManager::GetMouseButtonNames();
		char buffer[20] = {};
		InputManager::GetSingleton()->getMouse()->GetButtonName(inputDesc.button, buffer, 20);
		String key = buffer;
		Array<const char*, mouseKeys.size()> mouseKeysBuffer;
		int currentKey = 0;
		for (int i = 0; i < mouseKeys.size(); i++)
		{
			mouseKeysBuffer[i] = mouseKeys[i].c_str();
			currentKey = (key == mouseKeys[i]) ? i : currentKey;
		}
		ImGui::Combo("##mouseInput", &currentKey, mouseKeysBuffer.data(), mouseKeysBuffer.size());
		inputDesc.button = InputManager::GetSingleton()->getMouse()->GetButtonByName(mouseKeys[currentKey].c_str());
	}
	else if (inputDesc.device == Device::Keyboard)
	{
		inputDesc.button = originalDevice == inputDesc.device ? inputDesc.button : 0;
		auto keyboardKeys = InputManager::GetKeyboardButtonNames();
		char buffer[20] = {};
		InputManager::GetSingleton()->getKeyboard()->GetButtonName(inputDesc.button, buffer, 20);
		String key = buffer;
		Array<const char*, keyboardKeys.size()> keyboardKeysBuffer;
		int currentKey = 0;
		for (int i = 0; i < keyboardKeys.size(); i++)
		{
			keyboardKeysBuffer[i] = keyboardKeys[i].c_str();
			currentKey = (key == keyboardKeys[i]) ? i : currentKey;
		}
		ImGui::Combo("", &currentKey, keyboardKeysBuffer.data(), keyboardKeysBuffer.size());
		inputDesc.button = InputManager::GetSingleton()->getKeyboard()->GetButtonByName(keyboardKeys[currentKey].c_str());
	}
	else if (inputDesc.device == Device::Pad1 || inputDesc.device == Device::Pad2)
	{
		inputDesc.button = originalDevice == inputDesc.device ? inputDesc.button : 0;
		auto padKeys = InputManager::GetPadButtonNames();
		char buffer[20] = {};
		InputManager::GetSingleton()->getPad1()->GetButtonName(inputDesc.button, buffer, 20);
		String key = buffer;
		Array<const char*, padKeys.size()> padKeysBuffer;
		int currentKey = 0;
		for (int i = 0; i < padKeys.size(); i++)
		{
			padKeysBuffer[i] = padKeys[i].c_str();
			currentKey = (key == padKeys[i]) ? i : currentKey;
		}
		ImGui::Combo("##padInput", &currentKey, padKeysBuffer.data(), padKeysBuffer.size());
		inputDesc.button = InputManager::GetSingleton()->getPad1()->GetButtonByName(padKeys[currentKey].c_str());
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() / 3.0f);
	ImGui::InputTextWithHint("##event", "Event", &inputDesc.inputEvent);
}
