#include "diffuse_visual_component.h"

#include "resource_loader.h"

Component* DiffuseVisualComponent::Create(const LuaVariable& componentData)
{
	ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile(componentData["texturePath"].tostring());
	Ref<Texture> texture(new Texture(imageRes));

	Ref<DiffuseMaterial> material(new DiffuseMaterial(texture));

	DiffuseVisualComponent* diffuseComponent = new DiffuseVisualComponent(RenderPass::Global, material, ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"].tostring()));

	return diffuseComponent;
}

DiffuseVisualComponent::DiffuseVisualComponent(RenderPass renderPass, Ref<DiffuseMaterial> material, VisualModelResourceFile* resFile)
    : VisualComponent(renderPass, material, resFile)
    , m_DiffuseMaterial(material)
{
}

DiffuseVisualComponent::~DiffuseVisualComponent()
{
}
