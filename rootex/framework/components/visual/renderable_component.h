#pragma once

#include "component.h"
#include "framework/components/transform_component.h"
#include "renderer/material.h"
#include "scene.h"

class RenderableComponent : public Component
{
	DEPENDS_ON(TransformComponent);

	friend class ECSFactory;

protected:
	bool m_IsVisible;
	unsigned int m_RenderPass;

	bool m_LODEnable;
	float m_LODBias;
	float m_LODDistance;

	HashMap<Ref<Material>, Ref<Material>> m_MaterialOverrides;
	Vector<SceneID> m_AffectingStaticLightIDs;
	Vector<int> m_AffectingStaticLights;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerModelCB;

	RenderableComponent(
	    unsigned int renderPass,
	    const HashMap<String, String>& materialOverrides,
	    bool visibility,
	    bool lodEnable,
	    float lodBias,
	    float lodDistance,
	    const Vector<SceneID>& affectingStaticLightIDs);
	RenderableComponent(RenderableComponent&) = delete;
	virtual ~RenderableComponent() = default;

	float getLODFactor(float viewDistance);

public:
	static const ComponentID s_ComponentID = (ComponentID)ComponentIDs::RenderableComponent;

	virtual bool setupData() override;
	virtual bool setupEntities() override;

	virtual bool preRender(float deltaMilliseconds);
	bool isVisible() const;
	virtual void render(float viewDistance);
	virtual void postRender();

	virtual bool addAffectingStaticLight(SceneID id);
	virtual void removeAffectingStaticLight(SceneID id);

	void setVisibility(bool enabled);
	void setMaterialOverride(Ref<Material> oldMaterial, Ref<Material> newMaterial);

	unsigned int getRenderPass() const { return m_RenderPass; }

	virtual const char* getName() const override = 0;
	virtual ComponentID getComponentID() const override = 0;
	virtual JSON::json getJSON() const override;

	virtual void draw() override;
};
