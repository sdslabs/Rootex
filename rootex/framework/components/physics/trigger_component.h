#pragma once

#include "common/common.h"
#include "scene.h"
#include "collision_component.h"
#include "components/space/transform_component.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class TriggerComponent : public CollisionComponent
{
	DEFINE_COMPONENT(TriggerComponent);
	DEPENDS_ON(TransformComponent);

	Ref<btBoxShape> m_BoxShape;
	btGhostObject* m_Body = nullptr;

	Vector3 m_Dimensions;
	bool m_EntryRepeat;
	Vector<SceneID> m_EntryTargetIDs;

	bool m_ExitRepeat;
	Vector<SceneID> m_ExitTargetIDs;

	Vector<SceneID> m_EntryList;
	Vector<SceneID> m_LastUpdateEntryList;
	bool m_IsEntryNotified = false;
	bool m_IsExitNotified = false;

	void openRegister();
	void registerEntry(SceneID id);
	int findExitCount() const;
	void closeRegister();

	void notifyEntry();
	void notifyExit();
	bool isExitNotified() { return m_IsExitNotified; }
	bool isEntryNotified() { return m_IsEntryNotified; }
	bool canNotifyEntry() { return !isEntryNotified() || isEntryRepeat(); }
	bool canNotifyExit() { return !isExitNotified() || isExitRepeat(); }

	void addTarget(Vector<SceneID>& list, SceneID toAdd);
	void removeTarget(Vector<SceneID>& list, SceneID toRemove);

	void updateTransform();

	friend class TriggerSystem;

public:
	TriggerComponent(
	    const Vector3& dimensions,
	    bool entryRepeat,
	    const Vector<SceneID>& entryTargets,
	    bool exitRepeat,
	    const Vector<SceneID>& exitTargets,
	    int collisionGroup,
	    int collisionMask);
	~TriggerComponent() = default;

	bool isEntryRepeat() { return m_EntryRepeat; }
	bool isExitRepeat() { return m_ExitRepeat; }

	void setDimensions(const Vector3& dimensions);

	void addEntryTarget(SceneID toAdd);
	void addExitTarget(SceneID toAdd);
	void removeEntryTarget(SceneID toRemove);
	void removeExitTarget(SceneID toRemove);

	btGhostObject* getGhostObject() { return m_Body; };

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};
