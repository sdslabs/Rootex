#include "diffuse_visual_component.h"

#include "resource_loader.h"

Component* DiffuseVisualComponent::Create(const LuaVariable& componentData)
{
	ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile(componentData["texturePath"].tostring());
	Ref<Texture> texture(new Texture(imageRes));

	return nullptr;
}

DiffuseVisualComponent::~DiffuseVisualComponent()
{
}
