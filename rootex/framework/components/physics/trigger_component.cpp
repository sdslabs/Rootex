#include "trigger_component.h"

#include "scene.h"
#include "core/physics/bullet_conversions.h"
#include "systems/physics_system.h"

Ptr<Component> TriggerComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<TriggerComponent>(
	    componentData.value("dimensions", Vector3 { 0.5f, 0.5f, 0.5f }),
	    componentData.value("entryRepeat", false),
	    componentData.value("entryTargets", Vector<SceneID>()),
	    componentData.value("exitRepeat", false),
	    componentData.value("exitTargets", Vector<SceneID>()),
	    componentData.value("collisionGroup", (int)CollisionMask::TriggerVolume),
	    componentData.value("collisionMask", (int)CollisionMask::Player));
}

TriggerComponent::TriggerComponent(
    const Vector3& dimensions,
    bool entryRepeat,
    const Vector<SceneID>& entryTargets,
    bool exitRepeat,
    const Vector<SceneID>& exitTargets,
    int collisionGroup,
    int collisionMask)
    : CollisionComponent(collisionGroup, collisionMask)
    , m_EntryTargetIDs(entryTargets)
    , m_ExitTargetIDs(exitTargets)
    , m_EntryRepeat(entryRepeat)
    , m_ExitRepeat(exitRepeat)
    , m_Dimensions(dimensions)
    , m_DependencyOnTransformComponent(this)
{
}

void TriggerComponent::openRegister()
{
	m_EntryList.clear();
}

void TriggerComponent::registerEntry(SceneID id)
{
	m_EntryList.push_back(id);
}

void TriggerComponent::closeRegister()
{
	m_LastUpdateEntryList = std::move(m_EntryList);
}

int TriggerComponent::findExitCount() const
{
	int count = 0;
	for (const SceneID& lastEntry : m_LastUpdateEntryList)
	{
		for (const SceneID& currentEntry : m_EntryList)
		{
			if (lastEntry == currentEntry)
			{
				count++;
				break;
			}
		}
	}
	return m_LastUpdateEntryList.size() - count;
}

void TriggerComponent::notifyEntry()
{
	for (auto& entryID : m_EntryTargetIDs)
	{
		if (Scene* scene = Scene::FindSceneByID(entryID))
		{
			if (Entity* entity = scene->getEntity())
			{
				entity->call("enter", { getOwner() });
			}
		}
		else
		{
			WARN("Scene not found during enter trigger: " + std::to_string(entryID) + " from trigger entity " + getOwner()->getFullName());
		}
	}
	m_IsEntryNotified = true;
}

void TriggerComponent::notifyExit()
{
	for (auto& exitID : m_ExitTargetIDs)
	{
		if (Scene* scene = Scene::FindSceneByID(exitID))
		{
			if (Entity* entity = scene->getEntity())
			{
				entity->call("exit", { getOwner() });
			}
		}
		else
		{
			WARN("Scene not found during exit trigger: " + std::to_string(exitID) + " from trigger entity " + getOwner()->getFullName());
		}
	}
	m_IsExitNotified = true;
}

bool TriggerComponent::setupData()
{
	if (m_Body)
	{
		PhysicsSystem::GetSingleton()->removeCollisionObject(m_Body);
	}

	m_CollisionObject.reset(new btGhostObject());
	m_Body = (btGhostObject*)m_CollisionObject.get();

	m_BoxShape.reset(new btBoxShape(VecTobtVector3(m_Dimensions)));
	m_Body->setCollisionShape(m_BoxShape.get());
	m_Body->setUserPointer((CollisionComponent*)this);
	PhysicsSystem::GetSingleton()->addCollisionObject(m_Body, m_CollisionGroup, m_CollisionMask);

	m_Body->setCollisionFlags(m_Body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	updateTransform();

	return true;
}

void TriggerComponent::setDimensions(const Vector3& dimensions)
{
	m_Dimensions = dimensions;
	setupData();
}

void TriggerComponent::addTarget(Vector<SceneID>& list, SceneID toAdd)
{
	auto& findIt = std::find(list.begin(), list.end(), toAdd);
	if (findIt == list.end())
	{
		list.push_back(toAdd);
	}
}

void TriggerComponent::removeTarget(Vector<SceneID>& list, SceneID toRemove)
{
	auto& findIt = std::find(list.begin(), list.end(), toRemove);
	if (findIt != list.end())
	{
		list.erase(findIt);
	}
}

void TriggerComponent::updateTransform()
{
	btTransform transform = MatTobtTransform(m_TransformComponent->getRotationPosition());
	m_Body->activate(true);
	m_Body->setWorldTransform(transform);
}

void TriggerComponent::removeEntryTarget(SceneID toRemove)
{
	removeTarget(m_EntryTargetIDs, toRemove);
}

void TriggerComponent::removeExitTarget(SceneID toRemove)
{
	removeTarget(m_ExitTargetIDs, toRemove);
}

void TriggerComponent::addEntryTarget(SceneID toAdd)
{
	addTarget(m_EntryTargetIDs, toAdd);
}

void TriggerComponent::addExitTarget(SceneID toAdd)
{
	addTarget(m_ExitTargetIDs, toAdd);
}

JSON::json TriggerComponent::getJSON() const
{
	JSON::json j = CollisionComponent::getJSON();

	j["dimensions"] = m_Dimensions;
	j["entryRepeat"] = m_EntryRepeat;
	j["exitRepeat"] = m_ExitRepeat;
	j["entryTargets"] = m_EntryTargetIDs;
	j["exitTargets"] = m_ExitTargetIDs;

	return j;
}

void TriggerComponent::draw()
{
	PhysicsSystem::GetSingleton()->debugDrawComponent(
	    m_Body->getWorldTransform(),
	    m_BoxShape.get(),
	    VecTobtVector3({ 0.8f, 0.1f, 0.1f }));

	if (ImGui::DragFloat3("Dimensions", &m_Dimensions.x, 0.01f, 0.0f))
	{
		setDimensions(m_Dimensions);
	}

	ImGui::Checkbox("Entry Repeat", &m_EntryRepeat);

	int i = 0;
	if (ImGui::ListBoxHeader("Entry Targets"))
	{
		SceneID toRemove = -1;
		for (auto& entryTarget : m_EntryTargetIDs)
		{
			ImGui::PushID(i++);
			if (ImGui::Button("-"))
			{
				toRemove = entryTarget;
			}
			ImGui::SameLine();
			if (Scene* scene = Scene::FindSceneByID(entryTarget))
			{
				ImGui::Text("%s", scene->getFullName().c_str());
			}
			else
			{
				ImGui::Text("%d", entryTarget);
			}

			ImGui::PopID();
		}

		if (toRemove != -1)
		{
			removeEntryTarget(toRemove);
		}
		ImGui::ListBoxFooter();
	}

	enum class TargetAddMode
	{
		None,
		Entry,
		Exit
	};

	static TargetAddMode addMode = TargetAddMode::None;
	if (ImGui::Button("Add Entry Target"))
	{
		addMode = TargetAddMode::Entry;
		ImGui::OpenPopup("Select Entity");
	}

	ImGui::Checkbox("Exit Repeat", &m_ExitRepeat);

	if (ImGui::ListBoxHeader("Exit Targets"))
	{
		SceneID toRemove = -1;
		for (auto& exitTarget : m_ExitTargetIDs)
		{
			ImGui::PushID(i++);
			if (ImGui::Button("-"))
			{
				toRemove = exitTarget;
			}
			ImGui::SameLine();
			if (Scene* scene = Scene::FindSceneByID(exitTarget))
			{
				ImGui::Text("%s", scene->getFullName().c_str());
			}
			else
			{
				ImGui::Text("%d", exitTarget);
			}

			ImGui::PopID();
		}

		if (toRemove != -1)
		{
			removeExitTarget(toRemove);
		}
		ImGui::ListBoxFooter();
	}
	if (ImGui::Button("Add Exit Target"))
	{
		addMode = TargetAddMode::Exit;
		ImGui::OpenPopup("Select Entity");
	}

	if (ImGui::BeginPopup("Select Entity"))
	{
		for (const Scene* scene : Scene::FindAllScenes())
		{
			if (Entity* entity = scene->getEntity())
			{
				if (ImGui::MenuItem(scene->getFullName().c_str()))
				{
					switch (addMode)
					{
					case TargetAddMode::None:
						break;
					case TargetAddMode::Entry:
						addEntryTarget(scene->getID());
						break;
					case TargetAddMode::Exit:
						addExitTarget(scene->getID());
						break;
					default:
						break;
					}
				}
			}
		}

		ImGui::EndPopup();
	}

	CollisionComponent::draw();
}
