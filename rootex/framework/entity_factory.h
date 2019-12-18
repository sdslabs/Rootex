#pragma once

#include "common/common.h"
#include "resource_file.h"
#include "script/interpreter.h"
#include "entity.h"

class Component;
class Entity;

typedef Component* (*ComponentCreator)(const LuaVariable& componentDescription);
typedef unsigned int EntityID;

class EntityFactory
{
	static EntityID s_Count;

	LuaInterpreter m_LuaState;

	EntityID getNextID();

protected:
	HashMap<String, ComponentCreator> m_ComponentCreators;

	Ref<Component> createComponent(const String& name, const LuaVariable& componentData);

	EntityFactory();
	~EntityFactory() = default;

public:
	static EntityFactory* GetSingleton();

	Ref<Entity> createEntity(LuaTextResourceFile* actorLuaDescription);
};
