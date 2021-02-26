#include "particles_material.h"

#include "resource_files/image_resource_file.h"
#include "resource_loader.h"

#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"
#include "renderer/texture.h"

#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

ParticlesMaterial::ParticlesMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, const String& specularImagePath, bool isNormal, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky)
    : Material(ShaderLibrary::GetParticlesShader(), ParticlesMaterial::s_MaterialName, isAlpha)
    , m_ParticlesShader(ShaderLibrary::GetParticlesShader())
    , m_IsLit(isLit)
    , m_SpecularIntensity(specularIntensity)
    , m_SpecularPower(specularPower)
    , m_Reflectivity(reflectivity)
    , m_RefractionConstant(refractionConstant)
    , m_Refractivity(refractivity)
    , m_IsAffectedBySky(affectedBySky)
    , m_IsNormal(isNormal)
{
	m_DiffuseImageFile = ResourceLoader::CreateImageResourceFile(imagePath);
	setDiffuseTexture(m_DiffuseImageFile);
	if (isNormal)
	{
		m_NormalImageFile = ResourceLoader::CreateImageResourceFile(normalImagePath);
		setNormalTexture(m_NormalImageFile);
	}
	else
	{
		removeNormal();
	}
	setSpecularTexture(ResourceLoader::CreateImageResourceFile(specularImagePath));

	m_SamplerState = RenderingDevice::GetSingleton()->createSS();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void ParticlesMaterial::setPSConstantBuffer(const PSParticlesConstantBufferMaterial& constantBuffer)
{
	Material::SetPSConstantBuffer<PSParticlesConstantBufferMaterial>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], PER_OBJECT_PS_CPP);
}

Material* ParticlesMaterial::CreateDefault()
{
	return new ParticlesMaterial(false, "rootex/assets/white.png", "", "rootex/assets/white.png", false, true, 0.5f, 30.0f, 0.1f, 0.8f, 0.1f, true);
}

Material* ParticlesMaterial::Create(const JSON::json& materialData)
{
	bool isLit = materialData["isLit"];
	float specularIntensity = 2.0f;
	float specularPower = 30.0f;
	if (isLit)
	{
		specularIntensity = (float)materialData["specularIntensity"];
		specularPower = (float)materialData["specularPower"];
	}
	ParticlesMaterial* material = dynamic_cast<ParticlesMaterial*>(CreateDefault());
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
	bool isAlpha = false;
	if (materialData.find("isAlpha") != materialData.end())
	{
		isAlpha = materialData["isAlpha"];
	}
	bool isNormal = false;
	String normalImageFile = "";
	if (materialData.find("isNormal") != materialData.end())
	{
		isNormal = materialData["isNormal"];
		if (isNormal)
		{
			normalImageFile = materialData["normalImageFile"];
		}
	}
	String specularImageFile = "rootex/assets/white.png";
	if (materialData.find("specularImageFile") != materialData.end())
	{
		specularImageFile = materialData["specularImageFile"];
	}
	return new ParticlesMaterial(isAlpha, (String)materialData["imageFile"], normalImageFile, specularImageFile, isNormal, isLit, specularIntensity, specularPower, reflectivity, refractionConstant, refractivity, affectedBySky);
}

ID3D11ShaderResourceView* ParticlesMaterial::getPreview()
{
	return m_DiffuseImageFile->getTexture()->getTextureResourceView();
}

void ParticlesMaterial::bind()
{
	m_ParticlesShader->set(m_DiffuseImageFile->getTexture().get(), DIFFUSE_PS_CPP);
	if (m_IsNormal)
	{
		m_ParticlesShader->set(m_NormalImageFile->getTexture().get(), NORMAL_PS_CPP);
	}
	m_ParticlesShader->set(m_SpecularImageFile->getTexture().get(), SPECULAR_PS_CPP);

	PSParticlesConstantBufferMaterial objectPSCB;
	objectPSCB.affectedBySky = m_IsAffectedBySky;
	objectPSCB.hasNormalMap = m_IsNormal;
	objectPSCB.isLit = m_IsLit;
	objectPSCB.reflectivity = m_Reflectivity;
	objectPSCB.refractionConstant = m_RefractionConstant;
	objectPSCB.refractivity = m_Refractivity;
	objectPSCB.specularIntensity = m_SpecularIntensity;
	objectPSCB.specularPower = m_SpecularPower;

	setPSConstantBuffer(objectPSCB);
}

JSON::json ParticlesMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();

	j["imageFile"] = m_DiffuseImageFile->getPath().string();

	j["isLit"] = m_IsLit;
	j["specularImageFile"] = m_SpecularImageFile->getPath().generic_string();
	if (m_IsLit)
	{
		j["specularIntensity"] = m_SpecularIntensity;
		j["specularPower"] = m_SpecularPower;
	}
	j["isNormal"] = m_IsNormal;
	if (m_IsNormal)
	{
		j["normalImageFile"] = m_NormalImageFile->getPath().generic_string();
	}

	j["reflectivity"] = m_Reflectivity;
	j["refractionConstant"] = m_RefractionConstant;
	j["refractivity"] = m_Refractivity;
	j["affectedBySky"] = m_IsAffectedBySky;

	return j;
}

void ParticlesMaterial::setDiffuseTexture(ImageResourceFile* image)
{
	m_DiffuseImageFile = image;
}

void ParticlesMaterial::setNormalTexture(ImageResourceFile* image)
{
	m_IsNormal = true;
	m_NormalImageFile = image;
}

void ParticlesMaterial::setSpecularTexture(ImageResourceFile* image)
{
	m_SpecularImageFile = image;
}

void ParticlesMaterial::removeNormal()
{
	m_IsNormal = false;
	m_NormalImageFile = nullptr;
}

void ParticlesMaterial::draw()
{
	Material::draw();

	RootexSelectableImage("Diffuse Texture " ICON_ROOTEX_PENCIL_SQUARE_O, m_DiffuseImageFile, [this](const String& selectedFile) { setDiffuseTexture(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Diffuse Texture"))
	{
		setDiffuseTexture(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}
	RootexSelectableImage("Normal Texture " ICON_ROOTEX_PENCIL_SQUARE_O, m_NormalImageFile, [this](const String& selectedFile) { setNormalTexture(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Normal Texture"))
	{
		removeNormal();
	}
	RootexSelectableImage("Specular Texture " ICON_ROOTEX_PENCIL_SQUARE_O, m_SpecularImageFile, [this](const String& selectedFile) { setSpecularTexture(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Specular Texture"))
	{
		setSpecularTexture(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}

	ImGui::Checkbox("Affected by light", &m_IsLit);
	ImGui::DragFloat("##Specular Intensity", &m_SpecularIntensity, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	if (ImGui::Button("Specular Intensity"))
	{
		m_SpecularIntensity = 0.5f;
	}
	ImGui::DragFloat("##Specular Power", &m_SpecularPower, 1.0f, -100.0f, 100.0f);
	ImGui::SameLine();
	if (ImGui::Button("Specular Power"))
	{
		m_SpecularPower = 30.0f;
	}
	ImGui::Checkbox("Affected by sky", &m_IsAffectedBySky);
	ImGui::DragFloat("Reflectivity", &m_Reflectivity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Refraction Constant", &m_RefractionConstant, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Refractivity", &m_Refractivity, 0.01f, 0.0f, 1.0f);
}
