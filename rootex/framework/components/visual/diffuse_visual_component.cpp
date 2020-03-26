#include "diffuse_visual_component.h"

#include "resource_loader.h"

#include "framework/systems/render_system.h"
#include "framework/systems/light_system.h"
#include "framework/entity.h"

#include "core/renderer/material.h"

Component* DiffuseVisualComponent::Create(const JSON::json& componentData)
{
	ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile(componentData["texturePath"]);
	Ref<Texture> texture(new Texture(imageRes));

	Ref<DiffuseMaterial> material(new DiffuseMaterial(texture));

	VisualModelResourceFile* vmr = ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"]);
	DiffuseVisualComponent* diffuseComponent = new DiffuseVisualComponent(componentData["renderPass"], material, vmr, imageRes, texture.get());

	return diffuseComponent;
}

Component* DiffuseVisualComponent::CreateDefault()
{
	ImageResourceFile* imageRes = ResourceLoader::CreateImageResourceFile("rootex/assets/rootex.png");
	Ref<Texture> texture(new Texture(imageRes));

	Ref<DiffuseMaterial> material(new DiffuseMaterial(texture));

	VisualModelResourceFile* vmr = ResourceLoader::CreateVisualModelResourceFile("rootex/assets/cube.obj");
	DiffuseVisualComponent* diffuseComponent = new DiffuseVisualComponent(RenderPassMain | RenderPassEditor, material, vmr, imageRes, texture.get());

	return diffuseComponent;
}

DiffuseVisualComponent::DiffuseVisualComponent(const unsigned int& renderPass, Ref<DiffuseMaterial> material, VisualModelResourceFile* resFile, ImageResourceFile* imageRes, Texture* texture)
    : VisualComponent(renderPass, material, resFile)
    , m_ImageFile(imageRes)
{
#ifdef ROOTEX_EDITOR
	m_ImagePathUI = imageRes->getPath().string();
#endif // ROOTEX_EDITOR
}

DiffuseVisualComponent::~DiffuseVisualComponent()
{
}

bool DiffuseVisualComponent::preRender()
{
	if (m_Attributes.m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushMatrix(m_Attributes.getTransform());
		m_Attributes.setAbsoluteTransform(RenderSystem::GetSingleton()->getTopMatrix());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::ModelInverse, RenderSystem::GetSingleton()->getTopMatrix().Invert());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrix(Matrix::Identity);
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::ModelInverse, RenderSystem::GetSingleton()->getTopMatrix().Invert());
	}
	return true;
}

JSON::json DiffuseVisualComponent::getJSON() const
{
	JSON::json j;

	j["texturePath"] = m_ImageFile->getPath().string();
	j["resFile"] = m_Attributes.m_VisualModelResourceFile->getPath().string();
	j["renderPass"] = m_Attributes.m_RenderPassSetting;

	return j;
}

void DiffuseVisualComponent::setTexture(ImageResourceFile* image)
{
	Ref<Texture> texture(new Texture(image));
	m_Attributes.m_Material.reset(new DiffuseMaterial(texture));
	m_ImageFile = image;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void DiffuseVisualComponent::draw()
{
	VisualComponent::draw();
	
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
				WARN("Cannot assign a non-image file to Diffuse Visual Texture");
			}
		}
		ImGui::EndDragDropTarget();
	}
}
#endif // ROOTEX_EDITOR
