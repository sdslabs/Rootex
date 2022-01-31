#pragma once

#include "component.h"
#include "components/space/transform_component.h"
#include "resource_files/basic_material_resource_file.h"
#include "renderer/constant_buffer.h"
#include "scene.h"

class RenderableComponent : public Component
{
	DEPENDS_ON(TransformComponent);

protected:
	bool m_IsVisible;
	unsigned int m_RenderPass;

	bool m_LODEnable;
	float m_LODBias;
	float m_LODDistance;

	HashMap<MaterialResourceFile*, Ref<MaterialResourceFile>> m_MaterialOverrides;
	Vector<SceneID> m_AffectingStaticLightIDs;
	Vector<int> m_AffectingStaticLights;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerModelCB;

	RenderableComponent(Entity& owner, const JSON::json& data);

	float getLODFactor(float viewDistance);

public:
	virtual ~RenderableComponent() = default;

	void setVisible(bool enabled);
	bool isVisible() const;

	virtual bool preRender(float deltaMilliseconds);
	virtual void render(float viewDistance);
	virtual void postRender();

	virtual bool addAffectingStaticLight(SceneID id);
	virtual void removeAffectingStaticLight(SceneID id);

	virtual void setMaterialOverride(MaterialResourceFile* oldMaterial, Ref<MaterialResourceFile> newMaterial);
	Ref<MaterialResourceFile> getMaterialOverride(MaterialResourceFile* material) { return m_MaterialOverrides.at(material); }

	unsigned int getRenderPass() const { return m_RenderPass; }

	bool setupData() override;
	bool setupEntities() override;

	JSON::json getJSON() const override;
	void draw() override;
};
