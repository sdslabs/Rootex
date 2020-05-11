#include "textured_material.h"

#include "resource_loader.h"

#include "renderer/shader_library.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/texture.h"

TexturedMaterial::TexturedMaterial(const String& imagePath, float specularIntensity, float specularPower)
    : Material(ShaderLibrary::GetDiffuseShader(), TexturedMaterial::s_MaterialName)
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
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

void TexturedMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferLights& constantBuffer)
{
	Material::setPSConstantBuffer<PSDiffuseConstantBufferLights>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Lights], 3u);
}

void TexturedMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer)
{
	Material::setPSConstantBuffer<PSDiffuseConstantBufferMaterial>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], 4u);
}

void TexturedMaterial::setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

Material* TexturedMaterial::CreateDefault()
{
	return new TexturedMaterial("rootex/assets/rootex.png", 2.0f, 30.0f);
}

Material* TexturedMaterial::Create(const JSON::json& materialData)
{
	return new TexturedMaterial((String)materialData["imageFile"], (float)materialData["specularIntensity"], (float)materialData["specularPower"]);
}

void TexturedMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_DiffuseShader->set(m_DiffuseTexture.get());
	setPSConstantBuffer({ LightSystem::GetSingleton()->getLights() });
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ m_SpecularIntensity, m_SpecularPower }));
}

JSON::json TexturedMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();

	j["imageFile"] = m_ImageFile->getPath().string();
	j["specularIntensity"] = m_SpecularIntensity;
	j["specularPower"] = m_SpecularPower;

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
#endif // ROOTEX_EDITOR
