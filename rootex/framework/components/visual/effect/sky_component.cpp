#include "sky_component.h"

#include "resource_loader.h"
#include "resource_files/model_resource_file.h"
#include "renderer/material_library.h"

Ptr<Component> SkyComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<SkyComponent>(
	    ResourceLoader::CreateModelResourceFile(componentData.value("skySphere", "rootex/assets/sky.obj")),
	    std::dynamic_pointer_cast<SkyMaterial>(MaterialLibrary::GetMaterial(componentData.value("skyMaterial", "rootex/assets/materials/sky.rmat"))));
}

SkyComponent::SkyComponent(Ref<ModelResourceFile> skySphere, Ref<SkyMaterial> skyMaterial)
    : m_SkyMaterial(skyMaterial)
    , m_SkySphere(skySphere)
{
}

JSON::json SkyComponent::getJSON() const
{
	JSON::json j;

	j["skyMaterial"] = m_SkyMaterial->getFileName();
	j["skySphere"] = m_SkySphere->getPath().generic_string();

	return j;
}

void SkyComponent::draw()
{
	m_SkyMaterial->draw();
}
