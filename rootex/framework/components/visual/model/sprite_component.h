#pragma once

#include "model_component.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/basic_material_resource_file.h"

class SpriteComponent : public ModelComponent
{
	COMPONENT(SpriteComponent, Category::Model);

private:
	bool m_IsBillboarded;

	void adjustScaling();
	void setMaterialOverride(Ref<MaterialResourceFile> oldMaterial, Ref<MaterialResourceFile> newMaterial) override;

	static JSON::json InjectSpriteModel(const JSON::json& data);

public:
	SpriteComponent(Entity& owner, const JSON::json& data);
	~SpriteComponent() = default;

	Ref<MaterialResourceFile> getOverridingMaterialResourceFile();

	bool preRender(float deltaMilliseconds) override;
	void postRender() override;

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(SpriteComponent);