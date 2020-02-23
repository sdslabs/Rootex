#include "diffuse_visual_component.h"

#include "resource_loader.h"

#include "framework/components/visual/visual_component_graph.h"
#include "framework/entity.h"
#include "framework/systems/light_system.h"

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

bool DiffuseVisualComponent::preRender(VisualComponentGraph* graph)
{
	if (m_Attributes.m_TransformComponent)
	{
		graph->pushMatrix(m_Attributes.getTransform());
		m_Attributes.m_TransformComponent->m_TransformBuffer.m_AbsoluteTransform = graph->getTopMatrix();
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, graph->getTopMatrix());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::ModelInverse, graph->getTopMatrix().Invert());
	}
	else
	{
		graph->pushMatrix(Matrix::Identity);
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, graph->getTopMatrix());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::ModelInverse, graph->getTopMatrix().Invert());
	}
	return true;
}
