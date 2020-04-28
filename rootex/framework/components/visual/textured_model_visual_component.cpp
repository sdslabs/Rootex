#include "textured_model_visual_component.h"

#include "core/resource_loader.h"

#include "framework/entity.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"

#include "core/renderer/material.h"

Component* TexturedModelVisualComponent::Create(const JSON::json& componentData)
{
	//ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile(componentData["texturePath"]);
	//Ref<Texture> texture(new Texture(imageRes));

	Ref<TexturedMaterial> material(new TexturedMaterial());

	VisualModelResourceFile* vmr = ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"]);
	TexturedModelVisualComponent* diffuseComponent = new TexturedModelVisualComponent(componentData["renderPass"], material, vmr, componentData["isVisible"]);

	return diffuseComponent;
}

Component* TexturedModelVisualComponent::CreateDefault()
{
	//ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile("rootex/assets/rootex.png");
	//Ref<Texture> texture(new Texture(imageRes));

	Ref<TexturedMaterial> material(new TexturedMaterial());

	VisualModelResourceFile* vmr = ResourceLoader::CreateVisualModelResourceFile("rootex/assets/cube.obj");
	TexturedModelVisualComponent* diffuseComponent = new TexturedModelVisualComponent(RenderPassMain | RenderPassEditor, material, vmr, true);

	return diffuseComponent;
}

TexturedModelVisualComponent::TexturedModelVisualComponent(const unsigned int& renderPass, Ref<TexturedMaterial> material, VisualModelResourceFile* resFile, bool visibility)
    : ModelVisualComponent(renderPass, material, resFile, visibility)
{
}

TexturedModelVisualComponent::~TexturedModelVisualComponent()
{
}

void TexturedModelVisualComponent::render(RenderPass renderPass)
{
	if (renderPass & m_RenderPass)
	{
		RenderSystem::GetSingleton()->getRenderer()->draw(getVertexBuffer(), getIndexBuffer(), getMaterial());
	}
}

JSON::json TexturedModelVisualComponent::getJSON() const
{
	JSON::json& j = ModelVisualComponent::getJSON();

	//j["texturePath"] = m_ImageFile->getPath().string();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void TexturedModelVisualComponent::draw()
{
	ModelVisualComponent::draw();
}
#endif // ROOTEX_EDITOR
