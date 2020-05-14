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
    , m_Shader(ShaderLibrary::GetTextureShader())
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
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getCurrentMatrix()));
	m_Shader->set(m_DiffuseTexture.get());
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

#ifdef ROOTEX_EDITOR
#include "imgui_stdlib.h"
void BasicMaterial::draw(const String& id)
{
	ImGui::Text(BasicMaterial::s_MaterialName.c_str());

	m_ImagePathUI = m_ImageFile->getPath().string();
	if (ImGui::InputText((String("Texture##") + id).c_str(), &m_ImagePathUI, ImGuiInputTextFlags_EnterReturnsTrue))
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
		ImGui::ColorEdit4((String("Color##") + id).c_str(), &m_Color.x);
	}
	ImGui::Checkbox((String("Is Lit?##") + id).c_str(), &m_IsLit);
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
