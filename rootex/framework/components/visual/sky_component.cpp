#include "sky_component.h"

#include "resource_loader.h"
#include "renderer/material_library.h"

Component* SkyComponent::Create(const JSON::json& componentData)
{
	return new SkyComponent(componentData["skyMaterial"], componentData["skySphere"]);
}

Component* SkyComponent::CreateDefault()
{
	return new SkyComponent("rootex/assets/materials/sky.rmat", "rootex/assets/sky.obj");
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
	m_SkyMaterial->draw("0");
}
#endif // ROOTEX_EDITOR
