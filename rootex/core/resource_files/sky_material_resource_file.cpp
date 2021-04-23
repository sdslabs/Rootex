#include "sky_material_resource_file.h"

#include "resource_loader.h"
#include "framework/systems/render_system.h"

void to_json(JSON::json& j, const SkyMaterialData& s)
{
	j["sky"] = s.skyImage;
}

void from_json(const JSON::json& j, SkyMaterialData& s)
{
	s.skyImage = j.value("sky", String("rootex/assets/sky.dds"));
}

void SkyMaterialResourceFile::Load()
{
	BufferFormat skyFormat;
	skyFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	s_Shader.reset(new Shader("rootex/core/renderer/shaders/sky_vertex_shader.hlsl", "rootex/core/renderer/shaders/sky_pixel_shader.hlsl", skyFormat));

	s_Sampler = RenderingDevice::GetSingleton()->createSS(RenderingDevice::SamplerState::Default);
}

void SkyMaterialResourceFile::Destroy()
{
	s_Shader.reset();
	s_Sampler.Reset();
}

SkyMaterialResourceFile::SkyMaterialResourceFile(const FilePath& path)
    : MaterialResourceFile(Type::SkyMaterial, path)
{
	reimport();
}

void SkyMaterialResourceFile::setSky(Ref<ImageCubeResourceFile> sky)
{
	m_SkyFile = sky;
	m_MaterialData.skyImage = sky->getPath().generic_string();
}

void SkyMaterialResourceFile::bindShader()
{
	s_Shader->bind();
}

void SkyMaterialResourceFile::bindTextures()
{
	ID3D11ShaderResourceView* textures[] = {
		m_SkyFile->getTexture()->getTextureResourceView()
	};
	RenderingDevice::GetSingleton()->setPSSRV(SKY_PS_CPP, sizeof(textures) / sizeof(textures[0]), textures);
}

void SkyMaterialResourceFile::bindSamplers()
{
	RenderingDevice::GetSingleton()->setPSSS(SAMPLER_PS_CPP, 1, s_Sampler.GetAddressOf());
}

void SkyMaterialResourceFile::bindVSCB()
{
	RenderingDevice::GetSingleton()->editBuffer(
	    PerModelVSCBData(Matrix::CreateTranslation(
	        RenderSystem::GetSingleton()->getCamera()->getOwner().getComponent<TransformComponent>()->getAbsoluteTransform().Translation())),
	    m_VSCB.Get());
	RenderingDevice::GetSingleton()->setVSCB(PER_OBJECT_VS_CPP, 1, m_VSCB.GetAddressOf());
}

void SkyMaterialResourceFile::bindPSCB()
{
}

JSON::json SkyMaterialResourceFile::getJSON() const
{
	JSON::json j = MaterialResourceFile::getJSON();

	j.update(m_MaterialData);

	return j;
}

ID3D11ShaderResourceView* SkyMaterialResourceFile::getPreview() const
{
	return m_SkyFile->getTexture()->getTextureResourceView();
}

void SkyMaterialResourceFile::reimport()
{
	MaterialResourceFile::reimport();

	const JSON::json& j = OS::LoadFileContentsToJSONObject(getPath().generic_string());

	m_MaterialData = j;
	MaterialResourceFile::readJSON(j);

	m_SkyFile = ResourceLoader::CreateImageCubeResourceFile(m_MaterialData.skyImage);

	m_VSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelVSCBData>(PerModelVSCBData(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

bool SkyMaterialResourceFile::save()
{
	return saveMaterialData(getJSON());
}

void SkyMaterialResourceFile::draw()
{
	MaterialResourceFile::draw();
	RootexSelectableImageCube("Sky Texture" ICON_ROOTEX_EXTERNAL_LINK, m_SkyFile, [this](const String& selectedFile) { setSky(ResourceLoader::CreateImageCubeResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Sky Texture"))
	{
		setSky(ResourceLoader::CreateImageCubeResourceFile("rootex/assets/sky.dds"));
	}
}
