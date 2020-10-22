#pragma once

#include "component.h"
#include "entity.h"

class TriggerComponent : public Component
{
	static Component* Create(const JSON::json& componentData);

	friend class EntityFactory;

	Ref<TriggerComponent> m_TargetEntityTrigger;
	EntityID m_TargetEntityID;

	TriggerComponent(EntityID targetEntity);
	TriggerComponent(TriggerComponent&) = delete;
	~TriggerComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TriggerComponent;

	virtual bool setupEntities() override;

	void trigger();
	void setTarget(Ref<Entity> entity);

	virtual const char* getName() const override { return "TriggerComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif // ROOTEX_EDITOR
};
