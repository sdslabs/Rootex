#include "textured_model_visual_component.h"

#include "resource_loader.h"

#include "framework/entity.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"

#include "core/renderer/material.h"

Component* TexturedModelVisualComponent::Create(const JSON::json& componentData)
{
	ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile(componentData["texturePath"]);
	Ref<Texture> texture(new Texture(imageRes));

	Ref<TexturedMaterial> material(new TexturedMaterial(texture));

	VisualModelResourceFile* vmr = ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"]);
	TexturedModelVisualComponent* diffuseComponent = new TexturedModelVisualComponent(componentData["renderPass"], material, vmr, imageRes, texture.get(), componentData["isVisible"]);

	return diffuseComponent;
}

Component* TexturedModelVisualComponent::CreateDefault()
{
	ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile("rootex/assets/rootex.png");
	Ref<Texture> texture(new Texture(imageRes));

	Ref<TexturedMaterial> material(new TexturedMaterial(texture));

	VisualModelResourceFile* vmr = ResourceLoader::CreateVisualModelResourceFile("rootex/assets/cube.obj");
	TexturedModelVisualComponent* diffuseComponent = new TexturedModelVisualComponent(RenderPassMain | RenderPassEditor, material, vmr, imageRes, texture.get(), true);

	return diffuseComponent;
}

TexturedModelVisualComponent::TexturedModelVisualComponent(const unsigned int& renderPass, Ref<TexturedMaterial> material, VisualModelResourceFile* resFile, ImageResourceFile* imageRes, Texture* texture, bool visibility)
    : ModelVisualComponent(renderPass, material, resFile, visibility)
    , m_ImageFile(imageRes)
{
#ifdef ROOTEX_EDITOR
	m_ImagePathUI = imageRes->getPath().string();
#endif // ROOTEX_EDITOR
}

TexturedModelVisualComponent::~TexturedModelVisualComponent()
{
}

void TexturedModelVisualComponent::render(RenderPass renderPass)
{
	m_Material->setVertexShaderConstantBuffer(Material::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
	m_Material->setVertexShaderConstantBuffer(Material::VertexConstantBufferType::ModelInverse, RenderSystem::GetSingleton()->getTopMatrix().Invert());
	
	RenderSystem::GetSingleton()->getRenderer()->draw(getVertexBuffer(), getIndexBuffer(), getMaterial());
}

JSON::json TexturedModelVisualComponent::getJSON() const
{
	JSON::json& j = ModelVisualComponent::getJSON();

	j["texturePath"] = m_ImageFile->getPath().string();

	return j;
}

void TexturedModelVisualComponent::setTexture(ImageResourceFile* image)
{
	Ref<Texture> texture(new Texture(image));
	m_Material.reset(new TexturedMaterial(texture));
	m_ImageFile = image;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void TexturedModelVisualComponent::draw()
{
	ModelVisualComponent::draw();

	m_ImagePathUI = m_ImageFile->getPath().string();
	if (ImGui::InputText("Texture", &m_ImagePathUI, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(m_ImagePathUI);
		if (image)
		{
			setTexture(image);
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			FilePath ext = payloadPath.extension();
			if (ext == ".jpg" || ext == ".png" || ext == ".jpeg")
			{
				setTexture(ResourceLoader::CreateImageResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Cannot assign a non-image file to a texture");
			}
		}
		ImGui::EndDragDropTarget();
	}
}
#endif // ROOTEX_EDITOR
