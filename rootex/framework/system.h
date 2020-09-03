#pragma once

#include "common/common.h"

#include "entity.h"
#include "component.h"

/// ECS style System interface that allows iterating over components directly.
class System
{
	void sort();

protected:
	enum class UpdateOrder
	{
		Input,
		Update,
		Render,
		RenderUI,
		None
	};

	static Vector<System*> s_SystemStack;
	static HashMap<ComponentID, Vector<Component*>> s_Components;
	static void RegisterComponent(Component* component);
	static void DeregisterComponent(Component* component);
	
	friend class Entity;
	friend class EntityFactory;
	friend class Application;

	String m_SystemName;
	UpdateOrder m_UpdateOrder;

public:
	static const Vector<Component*>& GetComponents(ComponentID ID) { return s_Components[ID]; }
	
	System(const String& name, const UpdateOrder& order);
	System(System&) = delete;
	virtual ~System();

	virtual bool initialize(const JSON::json& systemData);
	virtual void update(float deltaMilliseconds);
	virtual void end();
	
	String getName() const { return m_SystemName; }
	const UpdateOrder& getUpdateOrder() const { return m_UpdateOrder; }

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
