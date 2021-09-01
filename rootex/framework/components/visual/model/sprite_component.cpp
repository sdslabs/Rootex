#include "sprite_component.h"

#include "common/common.h"

#include "event_manager.h"
#include "core/resource_loader.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(SpriteComponent);

JSON::json SpriteComponent::InjectSpriteModel(const JSON::json& data)
{
	JSON::json newData = JSON::json(data);
	newData["resFile"] = "rootex/assets/square.obj";
	return newData;
}

SpriteComponent::SpriteComponent(Entity& owner, const JSON::json& data)
    : ModelComponent(owner, SpriteComponent::InjectSpriteModel(data))
{
}

bool SpriteComponent::setupData()
{
	bool status = ModelComponent::setupData();

	if (status)
	{
		adjustScaling();
	}

	return status;
}

void SpriteComponent::adjustScaling()
{
	float defaultHeight = 1.0f;

	Ref<Texture> imageTexture = getSpriteMaterialResourceFile()->getTextures()[0];
	float aspectRatio = imageTexture->getWidth() / imageTexture->getHeight();

	getTransformComponent()->setScale(Vector3 { aspectRatio * defaultHeight, defaultHeight, 1.0f });
}

Ref<MaterialResourceFile> SpriteComponent::getSpriteMaterialResourceFile()
{
	auto& [material, meshes] = getMeshes()[0];
	return m_MaterialOverrides.at(material);
}

void SpriteComponent::setMaterialOverride(Ref<MaterialResourceFile> oldMaterial, Ref<MaterialResourceFile> newMaterial)
{
	RenderableComponent::setMaterialOverride(oldMaterial, newMaterial);
	adjustScaling();
}
