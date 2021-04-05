#pragma once

#include "component.h"
#include "components/space/transform_component.h"
#include "resource_files/basic_material_resource_file.h"
#include "renderer/constant_buffer.h"
#include "scene.h"

class RenderableComponent : public Component
{
	DEFINE_COMPONENT(RenderableComponent);
	DEPENDS_ON(TransformComponent);

protected:
	bool m_IsVisible;
	unsigned int m_RenderPass;

	bool m_LODEnable;
	float m_LODBias;
	float m_LODDistance;

	HashMap<Ref<MaterialResourceFile>, Ref<MaterialResourceFile>> m_MaterialOverrides;
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

	float getLODFactor(float viewDistance);

public:
	virtual ~RenderableComponent() = default;

	void setVisibility(bool enabled);
	bool isVisible() const;

	virtual bool preRender(float deltaMilliseconds);
	virtual void render(float viewDistance);
	virtual void postRender();

	virtual bool addAffectingStaticLight(SceneID id);
	virtual void removeAffectingStaticLight(SceneID id);

	void setMaterialOverride(Ref<MaterialResourceFile> oldMaterial, Ref<MaterialResourceFile> newMaterial);

	unsigned int getRenderPass() const { return m_RenderPass; }

	bool setupData() override;
	bool setupEntities() override;

	JSON::json getJSON() const override;
	void draw() override;
};
