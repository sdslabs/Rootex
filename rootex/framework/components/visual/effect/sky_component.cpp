#include "sky_component.h"

#include "resource_loader.h"
#include "resource_files/model_resource_file.h"

SkyComponent::SkyComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_SkyMaterial(ResourceLoader::CreateSkyMaterialResourceFile(data.value("skyMaterial", "rootex/assets/materials/default.sky.rmat")))
    , m_SkySphere(ResourceLoader::CreateModelResourceFile(data.value("skySphere", "rootex/assets/sky.obj")))
{
}

JSON::json SkyComponent::getJSON() const
{
	JSON::json j;

	j["skyMaterial"] = m_SkyMaterial->getPath().generic_string();
	j["skySphere"] = m_SkySphere->getPath().generic_string();

	return j;
}

void SkyComponent::draw()
{
	m_SkyMaterial->draw();
}
