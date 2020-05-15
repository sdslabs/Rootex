#include "basic_material.h"

#include "resource_loader.h"

#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"
#include "renderer/texture.h"

#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

BasicMaterial::BasicMaterial(const String& imagePath, Color color, bool isLit, float specularIntensity, float specularPower)
    : Material(ShaderLibrary::GetTextureShader(), BasicMaterial::s_MaterialName)
    , m_TextureShader(ShaderLibrary::GetTextureShader())
    , m_Color(color)
    , m_IsLit(isLit)
    , m_SpecularIntensity(specularIntensity)
    , m_SpecularPower(specularPower)
{
	m_ImageFile = ResourceLoader::CreateImageResourceFile(imagePath);
	setTexture(m_ImageFile);
	m_SamplerState = RenderingDevice::GetSingleton()->createSamplerState();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);

#ifdef ROOTEX_EDITOR
	m_ImagePathUI = imagePath;
#endif // ROOTEX_EDITOR
}

void BasicMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer)
{
	Material::setPSConstantBuffer<PSDiffuseConstantBufferMaterial>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], PER_OBJECT_PS_CPP);
}

void BasicMaterial::setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], PER_OBJECT_VS_CPP);
}

Material* BasicMaterial::CreateDefault()
{
	return new BasicMaterial("rootex/assets/white.png", Color(0.5f, 0.5f, 0.5f, 1.0f), true, 2.0f, 30.0f);
}

Material* BasicMaterial::Create(const JSON::json& materialData)
{
	bool isLit = materialData["isLit"];
	float specularIntensity = 2.0f;
	float specularPower = 30.0f;
	if (isLit)
	{
		specularIntensity = (float)materialData["specularIntensity"];
		specularPower = (float)materialData["specularPower"];
	}
	BasicMaterial* material = dynamic_cast<BasicMaterial*>(CreateDefault());
	material->m_IsLit = materialData["isLit"];
	if (material->m_IsLit)
	{
		material->m_SpecularIntensity = (float)materialData["specularIntensity"];
		material->m_SpecularPower = (float)materialData["specularPower"];
	}
	return new BasicMaterial((String)materialData["imageFile"], Color((float)materialData["color"]["r"], (float)materialData["color"]["g"], (float)materialData["color"]["b"], (float)materialData["color"]["a"]), isLit, specularIntensity, specularPower);
}

void BasicMaterial::bind()
{
	Material::bind();
	m_TextureShader->set(m_DiffuseTexture.get());
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getCurrentMatrix()));
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ m_Color, m_IsLit, m_SpecularIntensity, m_SpecularPower }));
}

JSON::json BasicMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();

	j["imageFile"] = m_ImageFile->getPath().string();

	j["color"]["r"] = m_Color.x;
	j["color"]["g"] = m_Color.y;
	j["color"]["b"] = m_Color.z;
	j["color"]["a"] = m_Color.w;
	j["isLit"] = m_IsLit;
	if (m_IsLit)
	{
		j["specularIntensity"] = m_SpecularIntensity;
		j["specularPower"] = m_SpecularPower;
	}

	return j;
}

void BasicMaterial::setTexture(ImageResourceFile* image)
{
	Ref<Texture> texture(new Texture(image));
	m_ImageFile = image;
	m_DiffuseTexture = texture;
}

void BasicMaterial::setTextureInternal(Ref<Texture> texture)
{
	m_DiffuseTexture = texture;
}

#ifdef ROOTEX_EDITOR
#include "imgui_stdlib.h"
void BasicMaterial::draw(const String& id)
{
	ImGui::Image(m_DiffuseTexture->getTextureResourceView(), { 50, 50 });
	ImGui::SameLine();
	ImGui::Text(m_ImageFile->getPath().string().c_str());
	ImGui::ColorButton((String("Color##") + id).c_str(), { m_Color.x, m_Color.y, m_Color.z, m_Color.w });
	ImGui::SameLine();
	ImGui::Text("Color");

	ImGui::Checkbox((String("Affected by light##") + id).c_str(), &m_IsLit);
	if (m_IsLit)
	{
		ImGui::DragFloat((String("##SpecularIntensity") + id).c_str(), &m_SpecularIntensity);
		ImGui::SameLine();
		if (ImGui::Button((String("Specular Intensity##") + id).c_str()))
		{
			m_SpecularIntensity = 2.0f;
		}
		ImGui::DragFloat((String("Specular Power##") + id).c_str(), &m_SpecularPower);
		ImGui::SameLine();
		if (ImGui::Button((String("Specular Power##") + id).c_str()))
		{
			m_SpecularPower = 30.0f;
		}
	}
}
#endif // ROOTEX_EDITOR
