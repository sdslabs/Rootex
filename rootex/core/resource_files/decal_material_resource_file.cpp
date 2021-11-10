#include "decal_material_resource_file.h"

#include "resource_loader.h"
#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"
#include "framework/systems/render_system.h"

void from_json(const JSON::json& j, DecalMaterialData& d)
{
	d.decalImage = j.value("decal", String("rootex/assets/white.png"));
	d.pixelBufferData.color = j.value("color", (Color)ColorPresets::White);
}

void to_json(JSON::json& j, const DecalMaterialData& d)
{
	j["decal"] = d.decalImage;
	j["color"] = d.pixelBufferData.color;
}

void DecalMaterialResourceFile::Load()
{
	BufferFormat basicBufferFormat;
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	basicBufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	s_Shader.reset(new Shader("rootex/core/renderer/shaders/basic_vertex_shader.hlsl", "rootex/core/renderer/shaders/decal_pixel_shader.hlsl", basicBufferFormat));

	s_Sampler = RenderingDevice::GetSingleton()->createSS(RenderingDevice::SamplerState::Default);
}

void DecalMaterialResourceFile::Destroy()
{
	s_Shader.reset();
	s_Sampler.Reset();
}

DecalMaterialResourceFile::DecalMaterialResourceFile(const Type type, const FilePath& path)
    : MaterialResourceFile(type, path)
{
}

DecalMaterialResourceFile::DecalMaterialResourceFile(const FilePath& path)
    : MaterialResourceFile(Type::DecalMaterial, path)
{
	reimport();
}

void DecalMaterialResourceFile::setColor(const Color& color)
{
	m_MaterialData.pixelBufferData.color = color;
}

void DecalMaterialResourceFile::setDecal(Ref<ImageResourceFile> decal)
{
	m_DecalImageFile = decal;
	m_MaterialData.decalImage = decal->getPath().generic_string();
}

void DecalMaterialResourceFile::bindShader()
{
	s_Shader->bind();
}

void DecalMaterialResourceFile::bindTextures()
{
	ID3D11ShaderResourceView* decalTexture[] = {
		m_DecalImageFile->getTexture()->getTextureResourceView()
	};
	RenderingDevice::GetSingleton()->setPSSRV(DIFFUSE_PS_CPP, 1, decalTexture);

	ID3D11ShaderResourceView* depthTexture[] = {
		RenderingDevice::GetSingleton()->getDepthSSRV().Get()
	};
	RenderingDevice::GetSingleton()->setPSSRV(DEPTH_TEXTURE_PS_CPP, 1, depthTexture);
}

void DecalMaterialResourceFile::bindSamplers()
{
	RenderingDevice::GetSingleton()->setPSSS(SAMPLER_PS_CPP, 1, s_Sampler.GetAddressOf());
}

void DecalMaterialResourceFile::bindVSCB()
{
	RenderingDevice::GetSingleton()->editBuffer(PerModelVSCBData(RenderSystem::GetSingleton()->getCurrentMatrix()), m_VSCB.Get());
	RenderingDevice::GetSingleton()->setVSCB(PER_OBJECT_VS_CPP, 1, m_VSCB.GetAddressOf());
}

void DecalMaterialResourceFile::bindPSCB()
{
	RenderingDevice::GetSingleton()->editBuffer(m_MaterialData.pixelBufferData, m_PSCB.Get());
	RenderingDevice::GetSingleton()->setPSCB(PER_OBJECT_PS_CPP, 1, m_PSCB.GetAddressOf());
}

JSON::json DecalMaterialResourceFile::getJSON() const
{
	JSON::json j = MaterialResourceFile::getJSON();

	j.update(m_MaterialData);

	return j;
}

ID3D11ShaderResourceView* DecalMaterialResourceFile::getPreview() const
{
	return m_DecalImageFile->getTexture()->getTextureResourceView();
}

void DecalMaterialResourceFile::reimport()
{
	MaterialResourceFile::reimport();

	const JSON::json& j = OS::LoadFileContentsToJSONObject(getPath().generic_string());

	m_MaterialData = j;
	MaterialResourceFile::readJSON(j);

	m_DecalImageFile = ResourceLoader::CreateImageResourceFile(m_MaterialData.decalImage);

	m_PSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelDecalPSCBData>(m_MaterialData.pixelBufferData, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	m_VSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelVSCBData>(PerModelVSCBData(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

bool DecalMaterialResourceFile::save()
{
	return saveMaterialData(getJSON());
}

void DecalMaterialResourceFile::draw()
{
	MaterialResourceFile::draw();

	RootexSelectableImage("Decal Texture " ICON_ROOTEX_FOLDER_OPEN, m_DecalImageFile, [this](const String& selectedFile) { setDecal(ResourceLoader::CreateImageResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Decal Texture"))
	{
		setDecal(ResourceLoader::CreateImageResourceFile("rootex/assets/white.png"));
	}

	ImGui::ColorEdit4("Color", &m_MaterialData.pixelBufferData.color.x, ImGuiColorEditFlags_PickerHueWheel);
}
