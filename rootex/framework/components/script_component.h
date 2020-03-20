#pragma once

#include "component.h"
#include "script/interpreter.h"

class ScriptComponent : public Component
{
public:
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

private:
	String m_EntityTable;
	LuaTextResourceFile* m_ScriptFile;

	friend class EntityFactory;

	ScriptComponent(LuaTextResourceFile* luaFile);
	ScriptComponent(ScriptComponent&) = delete;
	virtual ~ScriptComponent();

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ScriptComponent;

	virtual bool setup() override;

	void begin();
	virtual void update(float deltaMilliSeconds);
	void end();

	ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "ScriptComponent"; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
