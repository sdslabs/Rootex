#pragma once

#include "common/common.h"
#include "script/interpreter.h"
#include "components/component_ids.h"

typedef unsigned int ComponentID;
class Component;

class Dependable
{
public:
	virtual ComponentID getID() const = 0;
	virtual bool isValid() const = 0;
	virtual void setComponent(Component* component) = 0;
};

/// Depend on a component either softly (recoverable from dependency breakage) or harshly (cannot recover from breakage).
template <class ComponentType, bool isSoft>
class Dependency : public Dependable
{
	ComponentType* m_Component = nullptr;

public:
	Dependency(Component* dependedBy)
	{
		dependedBy->registerDependency(this);
	}
	Dependency(Dependency&&) = delete;
	Dependency(const Dependency&) = delete;
	~Dependency() = default;

	ComponentType*& getComponent() { return m_Component; }
	void setComponent(Component* component) override { m_Component = (ComponentType*)component; };

	/// Get the ID of the component which is depended upon. This should not use the component object because it is nullptr when this is called.
	ComponentID getID() const override { return ComponentType::s_ID; }

	/// Return if the dependency has been evaluated properly.
	bool isValid() const override { return isSoft || m_Component; }
};

#ifndef DEPENDS_ON
#define DEPENDS_ON(ComponentType)                                                     \
public:                                                                               \
	ComponentType* get##ComponentType() { return m_##ComponentType; }                 \
	ComponentType*& m_##ComponentType = m_DependencyOn##ComponentType.getComponent(); \
	Dependency<ComponentType, false> m_DependencyOn##ComponentType;                   \
                                                                                      \
private:
#endif

#ifndef SOFT_DEPENDS_ON
#define SOFT_DEPENDS_ON(ComponentType)                                                \
public:                                                                               \
	ComponentType* get##ComponentType() { return m_##ComponentType; }                 \
	ComponentType*& m_##ComponentType = m_DependencyOn##ComponentType.getComponent(); \
	Dependency<ComponentType, true> m_DependencyOn##ComponentType;                    \
                                                                                      \
private:
#endif

#define DEFINE_COMPONENT(componentName)                                       \
public:                                                                       \
	static const ComponentID s_ID = (ComponentID)ComponentIDs::componentName; \
	const char* getName() const override { return #componentName; }           \
	ComponentID getComponentID() const { return s_ID; }                       \
                                                                              \
private:                                                                      \
	static Ptr<Component> Create(const JSON::json& componentData);            \
	friend class ECSFactory;                                                  \
	componentName(const componentName&) = delete

/// An ECS style interface of a collection of data that helps implement a behaviour. Also allows operations on that data.
class Component
{
	Vector<Dependable*> m_Dependencies;

	/// Perform setting up dependencies and internal data. Return true if successful.
	bool setup();

	friend class ECSFactory;
	friend class Entity;

protected:
	Entity* m_Owner;

public:
	Component();
	virtual ~Component();

	/// Only use to register dependency through a Dependency object.
	void registerDependency(Dependable* dependable) { m_Dependencies.push_back(dependable); }
	const Vector<Dependable*>& getDependencies() const { return m_Dependencies; }

	/// Establish inter-component links after all components have been added on the owner entity. Return true if successful.
	bool resolveDependencies();
	/// Perform setting up internal data needed from other components after they have been added to the owning entity.
	virtual bool setupData();
	/// Perform setting up operations which are possible only after all entities have been set up.
	virtual bool setupEntities();
	/// Perform operations prior to detachment from owning entity and destruction.
	virtual void onRemove();

	Entity* getOwner() const;
	virtual ComponentID getComponentID() const = 0;
	virtual const char* getName() const = 0;
	/// Get JSON representation of the component data needed to re-construct component from memory.
	virtual JSON::json getJSON() const;

	/// Expose the component data with ImGui.
	virtual void draw();
};
