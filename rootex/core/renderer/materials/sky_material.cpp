#include "sky_material.h"

#include "resource_loader.h"

#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/shader_library.h"
#include "renderer/texture.h"

#include "renderer/shaders/register_locations_pixel_shader.h"
#include "renderer/shaders/register_locations_vertex_shader.h"

SkyMaterial::SkyMaterial(const String& imagePath)
    : Material(ShaderLibrary::GetSkyShader(), SkyMaterial::s_MaterialName, false)
    , m_SkyShader(ShaderLibrary::GetSkyShader())
{
	setSkyTexture(ResourceLoader::CreateImageCubeResourceFile(imagePath));
	m_SamplerState = RenderingDevice::GetSingleton()->createSS();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void SkyMaterial::setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::SetVSConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], PER_OBJECT_VS_CPP);
}

Material* SkyMaterial::CreateDefault()
{
	return new SkyMaterial("rootex/assets/sky.dds");
}

Material* SkyMaterial::Create(const JSON::json& materialData)
{
	return new SkyMaterial(materialData["imageFile"]);
}

void SkyMaterial::bind()
{
	m_SkyShader->setSkyTexture(m_SkyImage->getTexture());
	setVSConstantBuffer(VSDiffuseConstantBuffer(Matrix::CreateTranslation(RenderSystem::GetSingleton()->getCamera()->getOwner()->getComponent<TransformComponent>()->getAbsoluteTransform().Translation())));
}

JSON::json SkyMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();

	j["imageFile"] = m_SkyImage->getPath().generic_string();

	return j;
}

void SkyMaterial::setSkyTexture(ImageCubeResourceFile* skyImageFile)
{
	m_SkyImage = skyImageFile;
}

ID3D11ShaderResourceView* SkyMaterial::getPreview()
{
	return m_SkyImage->getTexture()->getTextureResourceView();
}

void SkyMaterial::draw()
{
	Material::draw();
	RootexSelectableImageCube("Sky Texture" ICON_ROOTEX_EXTERNAL_LINK, m_SkyImage, [this](const String& selectedFile) { setSkyTexture(ResourceLoader::CreateImageCubeResourceFile(selectedFile)); });
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Sky Texture"))
	{
		setSkyTexture(ResourceLoader::CreateImageCubeResourceFile("rootex/assets/sky.dds"));
	}
}
