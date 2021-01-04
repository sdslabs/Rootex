#include "sky_component.h"

#include "resource_loader.h"
#include "resource_files/model_resource_file.h"
#include "renderer/material_library.h"

Component* SkyComponent::Create(const JSON::json& componentData)
{
	return new SkyComponent(componentData.value("skyMaterial", "rootex/assets/materials/sky.rmat"), componentData.value("skySphere", "rootex/assets/sky.obj"));
}

SkyComponent::SkyComponent(const String& skyMaterialPath, const String& skySpherePath)
    : m_SkyMaterial(std::dynamic_pointer_cast<SkyMaterial>(MaterialLibrary::GetMaterial(skyMaterialPath)))
    , m_SkySphere(ResourceLoader::CreateModelResourceFile(skySpherePath))
{
}

JSON::json SkyComponent::getJSON() const
{
	JSON::json j;

	j["skyMaterial"] = m_SkyMaterial->getFileName();
	j["skySphere"] = m_SkySphere->getPath().generic_string();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void SkyComponent::draw()
{
	m_SkyMaterial->draw();
}
#endif // ROOTEX_EDITOR
