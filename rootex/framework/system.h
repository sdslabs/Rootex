#pragma once

#include "common/common.h"

#include "entity.h"
#include "component.h"

/// ECS style System interface that allows iterating over components directly.
class System
{
public:
	enum class UpdateOrder
	{
		Input,
		Update,
		Render,
		RenderUI,
		UI,
		PostRender,
		Editor,
		Async
	};

protected:
	static Map<UpdateOrder, Vector<System*>> s_Systems;
	static HashMap<ComponentID, Vector<Component*>> s_Components;
	static void RegisterComponent(Component* component);
	static void DeregisterComponent(Component* component);
	
	friend class Entity;
	friend class EntityFactory;

	String m_SystemName;
	UpdateOrder m_UpdateOrder;
	bool m_IsActive;

public:
	static const Map<UpdateOrder, Vector<System*>>& GetSystems() { return s_Systems; }
	static const Vector<Component*>& GetComponents(ComponentID ID) { return s_Components[ID]; }
	
	System(const String& name, const UpdateOrder& order, bool isGameplay);
	System(System&) = delete;
	virtual ~System();

	virtual bool initialize(const JSON::json& systemData);
	virtual void setConfig(const JSON::json& configData, bool openInEditor);
	virtual void begin();
	virtual void update(float deltaMilliseconds);
	virtual void end();
	
	String getName() const { return m_SystemName; }
	const UpdateOrder& getUpdateOrder() const { return m_UpdateOrder; }
	bool isActive() const { return m_IsActive; }

	void setActive(bool enabled);

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
