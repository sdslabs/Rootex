#include "textured_material.h"

#include "resource_loader.h"

#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"
#include "renderer/texture.h"

#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

TexturedMaterial::TexturedMaterial(const String& imagePath, Color color, bool isLit, float specularIntensity, float specularPower)
    : Material(ShaderLibrary::GetDiffuseShader(), TexturedMaterial::s_MaterialName)
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
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

void TexturedMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer)
{
	Material::setPSConstantBuffer<PSDiffuseConstantBufferMaterial>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], PER_OBJECT_PS_CPP);
}

void TexturedMaterial::setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], PER_OBJECT_VS_CPP);
}

Material* TexturedMaterial::CreateDefault()
{
	return new TexturedMaterial("rootex/assets/white.png", Color(1.0f, 1.0f, 1.0f, 1.0f), true, 2.0f, 30.0f);
}

Material* TexturedMaterial::Create(const JSON::json& materialData)
{
	bool isLit = materialData["isLit"];
	float specularIntensity = 2.0f;
	float specularPower = 30.0f;
	if (isLit)
	{
		specularIntensity = (float)materialData["specularIntensity"];
		specularPower = (float)materialData["specularPower"];
	}
	TexturedMaterial* material = dynamic_cast<TexturedMaterial*>(CreateDefault());
	material->m_IsLit = materialData["isLit"];
	if (material->m_IsLit)
	{
		material->m_SpecularIntensity = (float)materialData["specularIntensity"];
		material->m_SpecularPower = (float)materialData["specularPower"];
	}
	return new TexturedMaterial((String)materialData["imageFile"], Color((float)materialData["color"]["r"], (float)materialData["color"]["g"], (float)materialData["color"]["b"], (float)materialData["color"]["a"]), isLit, specularIntensity, specularPower);
}

void TexturedMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_DiffuseShader->set(m_DiffuseTexture.get());
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ m_Color, m_IsLit, m_SpecularIntensity, m_SpecularPower }));
}

JSON::json TexturedMaterial::getJSON() const
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

void TexturedMaterial::setTexture(ImageResourceFile* image)
{
	Ref<Texture> texture(new Texture(image));
	m_ImageFile = image;
	m_DiffuseTexture = texture;
}

#ifdef ROOTEX_EDITOR
#include "imgui_stdlib.h"
void TexturedMaterial::draw()
{
	ImGui::Text(TexturedMaterial::s_MaterialName.c_str());

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
	else
	{
		ImGui::ColorEdit4("Color", &m_Color.x);
	}
	ImGui::Checkbox("Is lit?", &m_IsLit);
	if (m_IsLit)
	{
		ImGui::DragFloat("##Specular Intensity", &m_SpecularIntensity);
		ImGui::SameLine();
		if (ImGui::Button("Specular Intensity"))
		{
			m_SpecularIntensity = 2.0f;
		}
		ImGui::DragFloat("##Specular Power", &m_SpecularPower);
		ImGui::SameLine();
		if (ImGui::Button("Specular Power"))
		{
			m_SpecularPower = 30.0f;
		}
	}
}
#endif // ROOTEX_EDITOR
