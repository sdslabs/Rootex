#include "decal_component.h"
#include "systems/render_system.h"
#include <rootex/vendor/Tracy/Tracy.hpp>

DEFINE_COMPONENT(DecalComponent);

DecalComponent::DecalComponent(Entity& owner, const JSON::json& data)
    : ModelComponent(owner, data)
{
	initDecalShader();
}

void DecalComponent::initDecalShader()
{
	BufferFormat basicBufferFormat;
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);

	m_DecalShader.reset(new Shader("rootex/core/renderer/shaders/decal_vertex_shader.hlsl", "rootex/core/renderer/shaders/decal_pixel_shader.hlsl", basicBufferFormat));
}

void DecalComponent::bindDepthTexture()
{
	ID3D11ShaderResourceView* textures[] = {
		RenderingDevice::GetSingleton()->getDepthSSRV().Get()
	};

	RenderingDevice::GetSingleton()->setPSSRV(DEPTH_TEXTURE_PS_CPP, 1, textures);
}

void DecalComponent::render(float viewDistance)
{
	ZoneNamedN(decalRender, "DecalRender", true);

	auto& [material, meshes] = m_ModelResourceFile->getMeshes()[0];

	m_DecalShader->bind();

	MaterialResourceFile* overridenMaterial = m_MaterialOverrides.at(material).get();
	overridenMaterial->bindSamplers();
	overridenMaterial->bindTextures();
	bindDepthTexture();
	overridenMaterial->bindVSCB();
	overridenMaterial->bindPSCB();

	auto& mesh = meshes[0];
	RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.getLOD(getLODFactor(viewDistance)).get());
}

void DecalComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);
	RenderableComponent::draw();
}
