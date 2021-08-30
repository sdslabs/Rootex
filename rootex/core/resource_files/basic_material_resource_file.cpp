#include "basic_material_resource_file.h"

#include "resource_loader.h"
#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"
#include "framework/systems/render_system.h"

void from_json(const JSON::json& j, BasicMaterialData& b)
{
	b.diffuseImage = j.value("diffuse", String("rootex/assets/white.png"));
	b.normalImage = j.value("normal", String("rootex/assets/white.png"));
	b.specularImage = j.value("specular", String("rootex/assets/white.png"));
	b.lightmapImage = j.value("lightmap", String("rootex/assets/white.png"));

	b.pixelBufferData.isLit = j.value("isLit", 1);
	b.pixelBufferData.color = j.value("color", (Color)ColorPresets::White);
	b.pixelBufferData.specularIntensity = j.value("specIntensity", 1.0f);
	b.pixelBufferData.specularPower = j.value("specPower", 30.0f);
	b.pixelBufferData.reflectivity = j.value("reflect", 0.0f);
	b.pixelBufferData.refractionConstant = j.value("refractionConstant", 0.8f);
	b.pixelBufferData.fresnelPower = j.value("fresnelPower", 0.8f);
	b.pixelBufferData.fresnelBrightness = j.value("fresnelBrightness", 0.8f);
	b.pixelBufferData.refractivity = j.value("refract", 0.0f);
	b.pixelBufferData.hasNormalMap = j.value("hasNormal", 0);
	b.pixelBufferData.affectedBySky = j.value("affectedBySky", 0);
}

void to_json(JSON::json& j, const BasicMaterialData& b)
{
	j["diffuse"] = b.diffuseImage;
	j["normal"] = b.normalImage;
	j["specular"] = b.specularImage;
	j["lightmap"] = b.lightmapImage;
	j["hasNormal"] = b.pixelBufferData.hasNormalMap;
	j["affectedBySky"] = b.pixelBufferData.affectedBySky;

	j["isLit"] = b.pixelBufferData.isLit;
	j["color"] = b.pixelBufferData.color;
	j["specIntensity"] = b.pixelBufferData.specularIntensity;
	j["specPower"] = b.pixelBufferData.specularPower;
	j["fresnelPower"] = b.pixelBufferData.fresnelPower;
	j["fresnelBrightness"] = b.pixelBufferData.fresnelBrightness;
	j["reflect"] = b.pixelBufferData.reflectivity;
	j["refractionConstant"] = b.pixelBufferData.refractionConstant;
	j["refract"] = b.pixelBufferData.refractivity;
}

void BasicMaterialResourceFile::Load()
{
	BufferFormat basicBufferFormat;
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	s_Shader.reset(new Shader("rootex/core/renderer/shaders/basic_vertex_shader.hlsl", "rootex/core/renderer/shaders/basic_pixel_shader.hlsl", basicBufferFormat));

	s_Sampler = RenderingDevice::GetSingleton()->createSS(RenderingDevice::SamplerState::Default);
}

void BasicMaterialResourceFile::Destroy()
{
	s_Shader.reset();
	s_Sampler.Reset();
}

BasicMaterialResourceFile::BasicMaterialResourceFile(const Type type, const FilePath& path)
    : MaterialResourceFile(type, path)
{
}

BasicMaterialResourceFile::BasicMaterialResourceFile(const FilePath& path)
    : MaterialResourceFile(Type::BasicMaterial, path)
{
	reimport();
}

void BasicMaterialResourceFile::setColor(const Color& color)
{
	m_MaterialData.pixelBufferData.color = color;
}

void BasicMaterialResourceFile::setDiffuse(Ref<ImageResourceFile> diffuse)
{
	m_DiffuseImageFile = diffuse;
	m_MaterialData.diffuseImage = diffuse->getPath().generic_string();
}

void BasicMaterialResourceFile::setNormal(Ref<ImageResourceFile> normal)
{
	if (!normal)
	{
		m_MaterialData.pixelBufferData.hasNormalMap = false;
		m_NormalImageFile = nullptr;
		return;
	}

	m_NormalImageFile = normal;
	m_MaterialData.normalImage = normal->getPath().generic_string();
	m_MaterialData.pixelBufferData.hasNormalMap = true;
}

void BasicMaterialResourceFile::setSpecular(Ref<ImageResourceFile> spec)
{
	m_SpecularImageFile = spec;
	m_MaterialData.specularImage = spec->getPath().generic_string();
}

void BasicMaterialResourceFile::setLightmap(Ref<ImageResourceFile> lightmap)
{
	m_LightmapImageFile = lightmap;
	m_MaterialData.lightmapImage = lightmap->getPath().generic_string();
}

Vector<Ref<Texture>> BasicMaterialResourceFile::getTextures() const
{
	return Vector<Ref<Texture>> {
		m_DiffuseImageFile->getTexture(),
		m_MaterialData.pixelBufferData.hasNormalMap ? m_NormalImageFile->getTexture() : nullptr,
		m_SpecularImageFile->getTexture(),
		m_LightmapImageFile->getTexture()
	};
}

void BasicMaterialResourceFile::bindShader()
{
	s_Shader->bind();
}

void BasicMaterialResourceFile::bindTextures()
{
	ID3D11ShaderResourceView* textures[] = {
		m_DiffuseImageFile->getTexture()->getTextureResourceView(),
		m_MaterialData.pixelBufferData.hasNormalMap ? m_NormalImageFile->getTexture()->getTextureResourceView() : nullptr,
		m_SpecularImageFile->getTexture()->getTextureResourceView(),
		m_LightmapImageFile->getTexture()->getTextureResourceView()
	};
	RenderingDevice::GetSingleton()->setPSSRV(DIFFUSE_PS_CPP, sizeof(textures) / sizeof(textures[0]), textures);
}

void BasicMaterialResourceFile::bindSamplers()
{
	RenderingDevice::GetSingleton()->setPSSS(SAMPLER_PS_CPP, 1, s_Sampler.GetAddressOf());
}

void BasicMaterialResourceFile::bindVSCB()
{
	RenderingDevice::GetSingleton()->editBuffer(PerModelVSCBData(RenderSystem::GetSingleton()->getCurrentMatrix()), m_VSCB.Get());
	RenderingDevice::GetSingleton()->setVSCB(PER_OBJECT_VS_CPP, 1, m_VSCB.GetAddressOf());
}

void BasicMaterialResourceFile::bindPSCB()
{
	RenderingDevice::GetSingleton()->editBuffer(m_MaterialData.pixelBufferData, m_PSCB.Get());
	RenderingDevice::GetSingleton()->setPSCB(PER_OBJECT_PS_CPP, 1, m_PSCB.GetAddressOf());
}

JSON::json BasicMaterialResourceFile::getJSON() const
{
	JSON::json j = MaterialResourceFile::getJSON();

	j.update(m_MaterialData);

	return j;
}

ID3D11ShaderResourceView* BasicMaterialResourceFile::getPreview() const
{
	return m_DiffuseImageFile->getTexture()->getTextureResourceView();
}

void BasicMaterialResourceFile::reimport()
{
	MaterialResourceFile::reimport();

	const JSON::json& j = OS::LoadFileContentsToJSONObject(getPath().generic_string());

	m_MaterialData = j;
	MaterialResourceFile::readJSON(j);

	m_DiffuseImageFile = ResourceLoader::CreateImageResourceFile(m_MaterialData.diffuseImage);
	m_NormalImageFile = ResourceLoader::CreateImageResourceFile(m_MaterialData.normalImage);
	m_SpecularImageFile = ResourceLoader::CreateImageResourceFile(m_MaterialData.specularImage);
	m_LightmapImageFile = ResourceLoader::CreateImageResourceFile(m_MaterialData.lightmapImage);

	m_PSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelPSCBData>(m_MaterialData.pixelBufferData, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	m_VSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelVSCBData>(PerModelVSCBData(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

bool BasicMaterialResourceFile::save()
{
	return saveMaterialData(getJSON());
}

void BasicMaterialResourceFile::draw()
{
	MaterialResourceFile::draw();

	RootexSelectableImage("Diffuse Texture " ICON_ROOTEX_FOLDER_OPEN, m_DiffuseImageFile, [this](const String& selectedFile) { setDiffuse(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Diffuse Texture"))
	{
		setDiffuse(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}
	RootexSelectableImage("Normal Texture " ICON_ROOTEX_FOLDER_OPEN, m_NormalImageFile, [this](const String& selectedFile) { setNormal(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Normal Texture"))
	{
		setNormal(nullptr);
	}
	RootexSelectableImage("Specular Texture " ICON_ROOTEX_FOLDER_OPEN, m_SpecularImageFile, [this](const String& selectedFile) { setSpecular(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Specular Texture"))
	{
		setSpecular(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}
	RootexSelectableImage("Lightmap Texture " ICON_ROOTEX_FOLDER_OPEN, m_LightmapImageFile, [this](const String& selectedFile) { setLightmap(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Lightmap Texture"))
	{
		setLightmap(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}

	ImGui::ColorEdit4("Color", &m_MaterialData.pixelBufferData.color.x, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::Checkbox("Affected by light", (bool*)&m_MaterialData.pixelBufferData.isLit);
	ImGui::DragFloat("##Specular Intensity", &m_MaterialData.pixelBufferData.specularIntensity, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	if (ImGui::Button("Specular Intensity"))
	{
		m_MaterialData.pixelBufferData.specularIntensity = 0.5f;
	}
	ImGui::DragFloat("##Specular Power", &m_MaterialData.pixelBufferData.specularPower, 1.0f, -100.0f, 100.0f);
	ImGui::SameLine();
	if (ImGui::Button("Specular Power"))
	{
		m_MaterialData.pixelBufferData.specularPower = 30.0f;
	}
	ImGui::Checkbox("Affected by sky", (bool*)&m_MaterialData.pixelBufferData.affectedBySky);
	ImGui::DragFloat("Reflectivity", &m_MaterialData.pixelBufferData.reflectivity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Refraction Constant", &m_MaterialData.pixelBufferData.refractionConstant, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Refractivity", &m_MaterialData.pixelBufferData.refractivity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Fresnel Power", &m_MaterialData.pixelBufferData.fresnelPower, 0.001f, 0.0f, 5.0f);
	ImGui::DragFloat("Fresnel Brightness", &m_MaterialData.pixelBufferData.fresnelBrightness, 0.01f, 1.0f, 100.0f);
}
