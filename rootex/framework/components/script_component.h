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
	Vector<sol::environment> m_ScriptEnvironments;
	Vector<String> m_ScriptFiles;

	friend class EntityFactory;

	ScriptComponent(const Vector<String>& luaFilePaths);
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
	virtual const char* getName() const override { return "ScriptComponent"; }
	virtual JSON::json getJSON() const override;

	void addScript(const String& scriptFile);
	void removeScript(const String& scriptFile);

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
