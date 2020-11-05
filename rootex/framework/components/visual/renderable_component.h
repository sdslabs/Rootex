#pragma once

#include "component.h"
#include "framework\components\hierarchy_component.h"
#include "framework\components\transform_component.h"
#include "renderer\material.h"

class RenderableComponent : public Component
{
	friend class EntityFactory;

protected:
	bool m_IsVisible;
	unsigned int m_RenderPass;

	TransformComponent* m_TransformComponent;
	HierarchyComponent* m_HierarchyComponent;

	HashMap<Ref<Material>, Ref<Material>> m_MaterialOverrides;
	Vector<EntityID> m_AffectingStaticLightEntityIDs;
	Vector<int> m_AffectingStaticLights;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerModelCB;

	RenderableComponent(unsigned int renderPass, const HashMap<String, String>& materialOverrides, bool isVisible, const Vector<EntityID>& affectingStaticLightEntityIDs);
	RenderableComponent(RenderableComponent&) = delete;
	virtual ~RenderableComponent() = default;

#ifdef ROOTEX_EDITOR
	/// Empty Vector means all materials are allowed
	Vector<String> m_AllowedMaterials;
#endif // ROOTEX_EDITOR

public:
	static void RegisterAPI(sol::table& rootex);
	static const ComponentID s_ComponentID = (ComponentID)ComponentIDs::RenderableComponent;

	virtual bool setup() override;
	virtual bool setupEntities() override;

	virtual bool preRender(float deltaMilliseconds = 0);
	bool isVisible() const;
	virtual void render();
	virtual void postRender();

	virtual bool addAffectingStaticLight(EntityID id);
	virtual void removeAffectingStaticLight(EntityID id);

	void setIsVisible(bool enabled);
	void setMaterialOverride(Ref<Material> oldMaterial, Ref<Material> newMaterial);

	unsigned int getRenderPass() const { return m_RenderPass; }

	virtual String getName() const override = 0;
	virtual ComponentID getComponentID() const override = 0;
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
