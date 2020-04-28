#include "textured_material.h"

#include "resource_loader.h"

#include "renderer/shader_library.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/texture.h"

TexturedMaterial::TexturedMaterial()
    : Material(ShaderLibrary::GetDiffuseShader())
    //, m_DiffuseTexture(diffuseTexture)
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
    //, m_ImageFile(imageRes)
{
	m_ImageFile = ResourceLoader::CreateImageResourceFile("rootex/assets/rootex.png");
	setTexture(m_ImageFile);
	m_SamplerState = RenderingDevice::GetSingleton()->createSamplerState();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
#ifdef ROOTEX_EDITOR
	m_ImagePathUI = "rootex/assets/rootex.png";
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

void TexturedMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_DiffuseShader->set(m_DiffuseTexture.get());
	setPSConstantBuffer({ LightSystem::GetSingleton()->getLights() });
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ 0.6f, 30.0f, { 0.0f, 0.0f } }));
}

void TexturedMaterial::setTexture(ImageResourceFile* image)
{
	Ref<Texture> texture(new Texture(image));
	m_ImageFile = image;
	m_DiffuseTexture = texture;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
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