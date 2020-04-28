#include "color_material.h"

#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"

ColorMaterial::ColorMaterial(): m_Color(1.0f, 0.0f, 0.0f)
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

void ColorMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSSolidConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	setPSConstantBuffer(PSSolidConstantBuffer({ m_Color }));
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ColorMaterial::draw()
{
	ImGui::Text("Color Material");
	ImGui::ColorEdit4("Color", &m_Color.x);
}
#endif // ROOTEX_EDITOR
