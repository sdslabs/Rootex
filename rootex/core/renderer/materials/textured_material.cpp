#include "textured_material.h"

#include "resource_loader.h"

#include "renderer/shader_library.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/texture.h"

TexturedMaterial::TexturedMaterial(const String& imagePath)
    : Material(ShaderLibrary::GetDiffuseShader(), "Textured Material")
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
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
	return new TexturedMaterial("rootex/assets/rootex.png");
}

Material* TexturedMaterial::Create(const JSON::json& materialData)
{
	return new TexturedMaterial((String)materialData["imageFile"]);
}

void TexturedMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_DiffuseShader->set(m_DiffuseTexture.get());
	setPSConstantBuffer({ LightSystem::GetSingleton()->getLights() });
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ 0.6f, 30.0f, { 0.0f, 0.0f } }));
}

JSON::json TexturedMaterial::getJSON() const
{
	JSON::json& j = Material::getJSON();

	j["imageFile"] = m_ImageFile->getPath();

	return j;
}

void TexturedMaterial::setTexture(ImageResourceFile* image)
{
	Ref<Texture> texture(new Texture(image));
	m_ImageFile = image;
	m_DiffuseTexture = texture;
}

#ifdef ROOTEX_EDITOR
void TexturedMaterial::draw()
{
	ImGui::Text("Textured Material");
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
}
#endif // ROOTEX_EDITOR
