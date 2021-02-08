#pragma once

#include "common/common.h"

#include "entity.h"
#include "component.h"
#include "scene.h"

#include "Tracy/Tracy.hpp"

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
		Async,
		End
	};

protected:
	static Vector<Vector<System*>> s_Systems;
	
	friend class Entity;

	String m_SystemName;
	UpdateOrder m_UpdateOrder;
	bool m_IsActive;

public:
	static const Vector<Vector<System*>>& GetSystems() { return s_Systems; }
	
	System(const String& name, const UpdateOrder& order, bool isGameplay);
	System(System&) = delete;
	virtual ~System();

	virtual bool initialize(const JSON::json& systemData);
	virtual void setConfig(const SceneSettings& sceneSettings);

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
