#include "diffuse_visual_component.h"

#include "resource_loader.h"

#include "framework/systems/render_system.h"
#include "framework/systems/light_system.h"
#include "framework/entity.h"

#include "core/renderer/material.h"

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

bool DiffuseVisualComponent::preRender(HierarchyGraph* graph)
{
	if (m_Attributes.m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushMatrix(m_Attributes.getTransform());
		m_Attributes.m_TransformComponent->m_TransformBuffer.m_AbsoluteTransform = RenderSystem::GetSingleton()->getTopMatrix();
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
