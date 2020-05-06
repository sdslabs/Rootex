#include "color_material.h"

#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"

ColorMaterial::ColorMaterial(const Color& color)
	: m_Color(color)
    , Material(ShaderLibrary::GetDefaultShader(), ColorMaterial::s_MaterialName)
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void ColorMaterial::setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	Material::setPSConstantBuffer<PSSolidConstantBuffer>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Color], 3u);
}

void ColorMaterial::setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

Material* ColorMaterial::CreateDefault()
{
	return new ColorMaterial({ 0.5f, 0.5f, 0.5f, 1.0f });
}

Material* ColorMaterial::Create(const JSON::json& materialData)
{
	return new ColorMaterial(Color(
		(float)materialData["color"]["r"], (float)materialData["color"]["g"],
		(float)materialData["color"]["b"], (float)materialData["color"]["a"]
	));
}

void ColorMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSSolidConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	setPSConstantBuffer(PSSolidConstantBuffer({ m_Color }));
}

JSON::json ColorMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();
	j["color"]["r"] = m_Color.x;
	j["color"]["g"] = m_Color.y;
	j["color"]["b"] = m_Color.z;
	j["color"]["a"] = m_Color.w;
	return j;
}

#ifdef ROOTEX_EDITOR
void ColorMaterial::draw()
{
	ImGui::Text(ColorMaterial::s_MaterialName.c_str());
	ImGui::ColorEdit4("Color", &m_Color.x);
}
#endif // ROOTEX_EDITOR
