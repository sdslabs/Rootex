#pragma once

#include "component.h"
#include "event_manager.h"
#include "script/interpreter.h"
#include "physics/physics_collider_component.h"

#include "btBulletDynamicsCommon.h"

class LuaTextResourceFile;

class ScriptComponent : public Component
{
public:
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

private:
	sol::environment m_ScriptEnvironment;
	String m_ScriptFile;
	HashMap<String, bool> m_IsOverriden;
	HashMap<String, String> m_Overrides;

	friend class EntityFactory;

	ScriptComponent(const String& luaFilePath);
	ScriptComponent(ScriptComponent&) = delete;
	virtual ~ScriptComponent();

	bool isSuccessful(const sol::function_result& result);

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ScriptComponent;

	virtual bool setup() override;

	void onBegin();
	virtual void onUpdate(float deltaMilliSeconds);
	void onEnd();
	void onHit(btPersistentManifold* manifold, PhysicsColliderComponent* other);

	ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "ScriptComponent"; }
	virtual JSON::json getJSON() const override;

	bool addScript(const String& scriptFile);
	void registerExports();
	void removeScript();

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
