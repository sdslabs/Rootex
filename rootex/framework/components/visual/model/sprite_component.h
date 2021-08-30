#pragma once

#include "model_component.h"
#include "core/resource_files/image_resource_file.h"
#include "core/resource_files/basic_material_resource_file.h"

class SpriteComponent : public ModelComponent
{
	COMPONENT(SpriteComponent, Category::Model);

private:
	void adjustScaling();
	void setMaterialOverride(Ref<MaterialResourceFile> oldMaterial, Ref<MaterialResourceFile> newMaterial) override;

	static JSON::json InjectSpriteModel(const JSON::json& data);

public:
	SpriteComponent(Entity& owner, const JSON::json& data);
	~SpriteComponent() = default;

	Ref<MaterialResourceFile> getSpriteMaterialResourceFile();

	bool setupData() override;
};

DECLARE_COMPONENT(SpriteComponent);
