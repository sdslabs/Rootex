#pragma once

#include "component.h"
#include "event_manager.h"
#include "script/interpreter.h"

class ScriptComponent : public Component
{
public:
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

private:
	sol::environment m_Env;
	LuaTextResourceFile* m_ScriptFile;
	Vector<String> m_Functions;
	HashMap<String, String> m_Connections;

	friend class EntityFactory;

	ScriptComponent(LuaTextResourceFile* luaFile, HashMap<String, String> connections = {});
	ScriptComponent(ScriptComponent&) = delete;
	virtual ~ScriptComponent();

	bool isSuccessful(const sol::function_result& result);

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ScriptComponent;

	virtual bool setup() override;

	void connect(const String& function, const String& eventType);
	void call(const String& function, const Event* event);
	void onBegin();
	virtual void onUpdate(float deltaMilliSeconds);
	void onEnd();

	LuaTextResourceFile* getScript() const { return m_ScriptFile; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "ScriptComponent"; }
	virtual JSON::json getJSON() const override;

	void setScript(LuaTextResourceFile* newScript);

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
