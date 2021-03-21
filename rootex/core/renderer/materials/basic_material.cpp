#include "basic_material.h"

#include "resource_loader.h"
#include "resource_files/image_resource_file.h"

#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"
#include "renderer/texture.h"

#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

BasicMaterial::BasicMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, const String& specularImagePath, const String& lightmapImagePath, bool isNormal, Color color, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky)
    : Material(ShaderLibrary::GetBasicShader(), BasicMaterial::s_MaterialName, isAlpha)
    , m_BasicShader(ShaderLibrary::GetBasicShader())
    , m_Color(color)
    , m_IsLit(isLit)
    , m_SpecularIntensity(specularIntensity)
    , m_SpecularPower(specularPower)
    , m_Reflectivity(reflectivity)
    , m_RefractionConstant(refractionConstant)
    , m_Refractivity(refractivity)
    , m_IsAffectedBySky(affectedBySky)
    , m_IsNormal(isNormal)
{
	setDiffuseTexture(ResourceLoader::CreateImageResourceFile(imagePath));
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
	setLightmapTexture(ResourceLoader::CreateImageResourceFile(lightmapImagePath));

	m_SamplerState = RenderingDevice::GetSingleton()->createSS();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void BasicMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer)
{
	Material::SetPSConstantBuffer<PSDiffuseConstantBufferMaterial>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], PER_OBJECT_PS_CPP);
}

void BasicMaterial::setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::SetVSConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], PER_OBJECT_VS_CPP);
}

Material* BasicMaterial::CreateDefault()
{
	return new BasicMaterial(false, "rootex/assets/white.png", "", "rootex/assets/white.png", "rootex/assets/white.png", false, Color(0.5f, 0.5f, 0.5f, 1.0f), true, 0.5f, 30.0f, 0.1f, 0.8f, 0.1f, true);
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
	String lightmapImageFile = "rootex/assets/white.png";
	if (materialData.find("lightmapImageFile") != materialData.end())
	{
		lightmapImageFile = materialData["lightmapImageFile"];
	}
	return new BasicMaterial(isAlpha, (String)materialData["imageFile"], normalImageFile, specularImageFile, lightmapImageFile, isNormal, Color((float)materialData["color"]["r"], (float)materialData["color"]["g"], (float)materialData["color"]["b"], (float)materialData["color"]["a"]), isLit, specularIntensity, specularPower, reflectivity, refractionConstant, refractivity, affectedBySky);
}

ID3D11ShaderResourceView* BasicMaterial::getPreview()
{
	return m_DiffuseImageFile->getTexture()->getTextureResourceView();
}

void BasicMaterial::bind()
{
	m_BasicShader->set(m_DiffuseImageFile->getTexture().get(), DIFFUSE_PS_CPP);
	if (m_IsNormal)
	{
		m_BasicShader->set(m_NormalImageFile->getTexture().get(), NORMAL_PS_CPP);
	}
	m_BasicShader->set(m_SpecularImageFile->getTexture().get(), SPECULAR_PS_CPP);
	m_BasicShader->set(m_LightmapImageFile->getTexture().get(), LIGHTMAP_PS_CPP);
	Matrix currentModelMatrix = RenderSystem::GetSingleton()->getCurrentMatrix();
	setVSConstantBuffer(VSDiffuseConstantBuffer(currentModelMatrix));

	PSDiffuseConstantBufferMaterial objectPSCB;
	objectPSCB.affectedBySky = m_IsAffectedBySky;
	objectPSCB.color = m_Color;
	objectPSCB.hasNormalMap = m_IsNormal;
	objectPSCB.isLit = m_IsLit;
	objectPSCB.reflectivity = m_Reflectivity;
	objectPSCB.refractionConstant = m_RefractionConstant;
	objectPSCB.refractivity = m_Refractivity;
	objectPSCB.specularIntensity = m_SpecularIntensity;
	objectPSCB.specularPower = m_SpecularPower;

	setPSConstantBuffer(objectPSCB);
}

JSON::json BasicMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();

	j["imageFile"] = m_DiffuseImageFile->getPath().string();

	j["color"]["r"] = m_Color.x;
	j["color"]["g"] = m_Color.y;
	j["color"]["b"] = m_Color.z;
	j["color"]["a"] = m_Color.w;
	j["isLit"] = m_IsLit;
	j["specularImageFile"] = m_SpecularImageFile->getPath().generic_string();
	j["lightmapImageFile"] = m_LightmapImageFile->getPath().generic_string();
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

void BasicMaterial::setDiffuseTexture(Ref<ImageResourceFile> image)
{
	if (!image)
	{
		WARN("Null image tried to be set as diffuse texture");
		return;
	}
	m_DiffuseImageFile = image;
}

void BasicMaterial::setNormalTexture(Ref<ImageResourceFile> image)
{
	if (!image)
	{
		WARN("Null image tried to be set as normal texture");
		return;
	}
	m_IsNormal = true;
	m_NormalImageFile = image;
}

void BasicMaterial::setSpecularTexture(Ref<ImageResourceFile> image)
{
	if (!image)
	{
		WARN("Null image tried to be set as specular texture");
		return;
	}
	m_SpecularImageFile = image;
}

void BasicMaterial::setLightmapTexture(Ref<ImageResourceFile> image)
{
	if (!image)
	{
		WARN("Null image tried to be set as lightmap texture");
		return;
	}
	m_LightmapImageFile = image;
}

void BasicMaterial::removeNormal()
{
	m_IsNormal = false;
	m_NormalImageFile = nullptr;
}

void BasicMaterial::draw()
{
	Material::draw();

	RootexSelectableImage("Diffuse Texture " ICON_ROOTEX_PENCIL_SQUARE_O, m_DiffuseImageFile.get(), [this](const String& selectedFile) { setDiffuseTexture(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Diffuse Texture"))
	{
		setDiffuseTexture(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}
	RootexSelectableImage("Normal Texture " ICON_ROOTEX_PENCIL_SQUARE_O, m_NormalImageFile.get(), [this](const String& selectedFile) { setNormalTexture(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Normal Texture"))
	{
		removeNormal();
	}
	RootexSelectableImage("Specular Texture " ICON_ROOTEX_PENCIL_SQUARE_O, m_SpecularImageFile.get(), [this](const String& selectedFile) { setSpecularTexture(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Specular Texture"))
	{
		setSpecularTexture(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}
	RootexSelectableImage("Lightmap Texture " ICON_ROOTEX_PENCIL_SQUARE_O, m_LightmapImageFile.get(), [this](const String& selectedFile) { setLightmapTexture(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Lightmap Texture"))
	{
		setLightmapTexture(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}

	ImGui::ColorEdit4("Color", &m_Color.x, ImGuiColorEditFlags_PickerHueWheel);
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
