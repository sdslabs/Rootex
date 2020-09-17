#include "animated_material.h"

#include "renderer/shaders/register_locations_vertex_shader.h"
#include "resource_loader.h"
#include "renderer/shader_library.h"
#include "framework/systems/render_system.h"

AnimatedMaterial::AnimatedMaterial(const String& imagePath, Color color, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky)
    : Material(ShaderLibrary::GetAnimationShader(), AnimatedMaterial::s_MaterialName)
    , m_AnimationShader(ShaderLibrary::GetAnimationShader())
    , m_Color(color)
    , m_IsLit(isLit)
    , m_SpecularIntensity(specularIntensity)
    , m_SpecularPower(specularPower)
    , m_Reflectivity(reflectivity)
    , m_RefractionConstant(refractionConstant)
    , m_Refractivity(refractivity)
    , m_IsAffectedBySky(affectedBySky)
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

void AnimatedMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantbuffer)
{
	Material::SetPSConstantBuffer<PSDiffuseConstantBufferMaterial>(constantbuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], PER_OBJECT_PS_CPP);
}

void AnimatedMaterial::setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::SetVSConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], PER_OBJECT_VS_CPP);
}

void AnimatedMaterial::setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer)
{
	Material::SetVSConstantBuffer<VSAnimationConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Animation], PER_FRAME_VS_CPP);
}

Material* AnimatedMaterial::CreateDefault()
{
	return new AnimatedMaterial("rootex/assets/white.png", Color(0.5f, 0.5f, 0.5f, 1.0f), false, 2.0f, 30.0f, 0.5f, 0.8f, 0.5f, false);
}

Material* AnimatedMaterial::Create(const JSON::json& materialData)
{
	bool isLit = materialData["isLit"];
	float specularIntensity = 2.0f;
	float specularPower = 30.0f;
	if (isLit)
	{
		specularIntensity = (float)materialData["specularIntensity"];
		specularPower = (float)materialData["specularPower"];
	}
	AnimatedMaterial* material = dynamic_cast<AnimatedMaterial*>(CreateDefault());
	material->m_IsLit = materialData["isLit"];
	if (material->m_IsLit)
	{
		material->m_SpecularIntensity = (float)materialData["specularIntensity"];
		material->m_SpecularPower = (float)materialData["specularPower"];
	}
	float reflectivity = 0.5f;
	if (materialData.find("reflectivity") != materialData.end())
	{
		reflectivity = materialData["reflectivity"];
	}
	float refractionConstant = 0.0f;
	if (materialData.find("refractionConstant") != materialData.end())
	{
		refractionConstant = materialData["refractionConstant"];
	}
	float refractivity = 0.0f;
	if (materialData.find("refractivity") != materialData.end())
	{
		refractivity = materialData["refractivity"];
	}
	bool affectedBySky = false;
	if (materialData.find("affectedBySky") != materialData.end())
	{
		affectedBySky = materialData["affectedBySky"];
	}
	return new AnimatedMaterial((String)materialData["imageFile"], Color((float)materialData["color"]["r"], (float)materialData["color"]["b"], (float)materialData["color"]["g"], (float)materialData["color"]["a"]), isLit, specularIntensity, specularPower, reflectivity, refractionConstant, refractivity, affectedBySky);
}

void AnimatedMaterial::bind()
{
	Material::bind();
	m_AnimationShader->setTexture(m_DiffuseTexture.get());
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getCurrentMatrix()));
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ m_Color, m_IsLit, m_SpecularIntensity, m_SpecularPower, m_Reflectivity, m_RefractionConstant, m_Refractivity, m_IsAffectedBySky }));
}

JSON::json AnimatedMaterial::getJSON() const
{
	JSON::json j = Material::getJSON();

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
	j["reflectivity"] = m_Reflectivity;
	j["refractionConstant"] = m_RefractionConstant;
	j["refractivity"] = m_Refractivity;
	j["affectedBySky"] = m_IsAffectedBySky;

	return j;
}

void AnimatedMaterial::setTexture(ImageResourceFile* image)
{
	Ref<Texture> texture(new Texture(image));
	m_ImageFile = image;
	m_DiffuseTexture = texture;
}

void AnimatedMaterial::setTextureInternal(Ref<Texture> texture)
{
	m_DiffuseTexture = texture;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AnimatedMaterial::draw(const String& id)
{
	Material::draw(id);

	ImGui::BeginGroup();
	ImGui::Image(m_DiffuseTexture->getTextureResourceView(), { 50, 50 });
	ImGui::SameLine();
	ImGui::Text(m_ImageFile->getPath().string().c_str());
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (IsFileSupported(payloadPath.extension().string(), ResourceFile::Type::Image))
			{
				ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(payloadPath.generic_string());
				
				if (image)
				{
					setTexture(image);
				}
			}
			else
			{
				WARN("Cannot assign a non-image file to texture");
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::ColorEdit4((String("Color##") + id).c_str(), &m_Color.x);

	ImGui::Checkbox((String("Affected by light") + id).c_str(), &m_IsLit);
	ImGui::DragFloat((String("##Specular Intensity") + id).c_str(), &m_SpecularIntensity);
	ImGui::SameLine();
	if (ImGui::Button((String("Specular Intensity##") + id).c_str()))
	{
		m_SpecularIntensity = 2.0f;
	}
	ImGui::DragFloat((String("##SpecularPower") + id).c_str(), &m_SpecularPower);
	ImGui::SameLine();
	if (ImGui::Button((String("Specular Power##") + id).c_str()))
	{
		m_SpecularPower = 30.0f;
	}

	ImGui::Checkbox((String("Affected by sky##") + id).c_str(), &m_IsAffectedBySky);
	ImGui::DragFloat((String("Reflectivity##") + id).c_str(), &m_Reflectivity);
	ImGui::DragFloat((String("Refraction Constant##") + id).c_str(), &m_RefractionConstant);
	ImGui::DragFloat((String("Refractivity##") + id).c_str(), &m_Refractivity);
}
#endif // ROOTEX_EDITOR
