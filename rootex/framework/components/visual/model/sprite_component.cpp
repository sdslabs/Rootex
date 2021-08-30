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
	float m_ImageScaleFactor = 0.001;

	Ref<Texture> imageTexture = getSpriteMaterialResourceFile()->getTextures()[0];
	float rectWidth = imageTexture->getWidth() * m_ImageScaleFactor;
	float rectHeight = imageTexture->getHeight() * m_ImageScaleFactor;

	getTransformComponent()->setScale(Vector3 { rectWidth, rectHeight, 1.0f });
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
