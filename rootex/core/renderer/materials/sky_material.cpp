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
	setTexture(ResourceLoader::CreateImageResourceFile(imagePath));
	m_SamplerState = RenderingDevice::GetSingleton()->createSS();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);

#ifdef ROOTEX_EDITOR
	m_ImagePathUI = imagePath;
#endif // ROOTEX_EDITOR
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
	m_SkyShader->setSkyTexture(m_SkyTexture.get());
	setVSConstantBuffer(VSDiffuseConstantBuffer(Matrix::CreateTranslation(RenderSystem::GetSingleton()->getCamera()->getOwner()->getComponent<TransformComponent>()->getAbsoluteTransform().Translation())));
}

JSON::json SkyMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();

	j["imageFile"] = m_SkyTexture->getImage()->getPath().string();

	return j;
}

void SkyMaterial::setTexture(ImageResourceFile* image)
{
	Ref<Texture3D> texture(new Texture3D(image));
	m_SkyTexture = texture;
}

ID3D11ShaderResourceView* SkyMaterial::getPreview()
{
	return m_SkyTexture->getTextureResourceView();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void SkyMaterial::draw(const String& id)
{
	Material::draw(id);

	ImGui::BeginGroup();
	ImGui::Text(m_SkyTexture->getImage()->getPath().string().c_str());
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
}
#endif // ROOTEX_EDITOR
