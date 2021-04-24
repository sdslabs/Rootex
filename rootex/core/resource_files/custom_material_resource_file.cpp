#include "custom_material_resource_file.h"

#include "application.h"
#include "core/renderer/shaders/register_locations_pixel_shader.h"
#include "core/renderer/shaders/register_locations_vertex_shader.h"
#include "framework/systems/render_system.h"
#include "resource_loader.h"

void to_json(JSON::json& j, const CustomMaterialData& s)
{
	j["pixelShader"] = s.pixelShaderPath;
	j["textures"] = JSON::json::array();
	for (auto& texture : s.textures)
	{
		j["textures"].push_back(texture->getPath().generic_string());
	}
}

void from_json(const JSON::json& j, CustomMaterialData& s)
{
	s.pixelShaderPath = j.value("pixelShader", CustomMaterialResourceFile::s_DefaultCustomShaderPath);
	for (auto& texturePath : j.value("textures", Vector<String>({ "rootex/assets/rootex.png" })))
	{
		if (Ref<ImageResourceFile> texture = ResourceLoader::CreateImageResourceFile(texturePath))
		{
			s.textures.push_back(texture);
		}
	}
}

void CustomMaterialResourceFile::pushTexture(Ref<ImageResourceFile> texture)
{
	if (!texture)
	{
		WARN("Skipping null texture which was tried to be pushed");
		return;
	}

	m_MaterialData.textures.push_back(texture);
}

void CustomMaterialResourceFile::setTexture(const String& newtexturePath, int position)
{
	if (position >= m_MaterialData.textures.size() || position < 0)
	{
		WARN("Position being set is out of bounds: " + std::to_string(position));
		return;
	}

	if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(newtexturePath))
	{
		m_MaterialData.textures[position] = image;
	}
}

void CustomMaterialResourceFile::popTexture()
{
	if (m_MaterialData.textures.empty())
	{
		WARN("We don't pop anymore. Texture list is empty.");
		return;
	}

	m_MaterialData.textures.pop_back();
}

void CustomMaterialResourceFile::Load()
{
	s_Sampler = RenderingDevice::GetSingleton()->createSS(RenderingDevice::SamplerState::Default);
}

void CustomMaterialResourceFile::Destroy()
{
	s_Sampler.Reset();
}

CustomMaterialResourceFile::CustomMaterialResourceFile(const FilePath& path)
    : MaterialResourceFile(ResourceFile::Type::CustomMaterial, path)
{
	reimport();
}

void CustomMaterialResourceFile::setPixelShader(const String& pixelShader)
{
	if (!OS::IsExists(pixelShader))
	{
		ERR("Custom pixel shader not found: " + pixelShader);
		return;
	}

	BufferFormat bufferFormat;
	bufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	bufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	bufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	bufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	Ptr<Shader> shader(new Shader("rootex/core/renderer/shaders/basic_vertex_shader.hlsl", pixelShader, bufferFormat));

	if (shader->isValid())
	{
		m_MaterialData.pixelShaderPath = pixelShader;
		m_Shader = std::move(shader);
	}
}

void CustomMaterialResourceFile::bindShader()
{
	m_Shader->bind();
}

void CustomMaterialResourceFile::bindTextures()
{
}

void CustomMaterialResourceFile::bindSamplers()
{
	RenderingDevice::GetSingleton()->setPSSS(SAMPLER_PS_CPP, 1, s_Sampler.GetAddressOf());
}

void CustomMaterialResourceFile::bindVSCB()
{
	RenderingDevice::GetSingleton()->editBuffer(PerModelVSCBData(RenderSystem::GetSingleton()->getCurrentMatrix()), m_VSCB.Get());
	RenderingDevice::GetSingleton()->setVSCB(PER_OBJECT_VS_CPP, 1, m_VSCB.GetAddressOf());
}

void CustomMaterialResourceFile::bindPSCB()
{
	if (!m_MaterialData.textures.empty())
	{
		Vector<ID3D11ShaderResourceView*> textureSRVs;
		for (auto& texture : m_MaterialData.textures)
		{
			textureSRVs.push_back(texture->getTexture()->getTextureResourceView());
		}

		RenderingDevice::GetSingleton()->setPSSRV(CUSTOM_TEXTURE_0_PS_CPP, textureSRVs.size(), textureSRVs.data());
	}
}

JSON::json CustomMaterialResourceFile::getJSON() const
{
	JSON::json j = MaterialResourceFile::getJSON();

	j.update(m_MaterialData);

	return j;
}

ID3D11ShaderResourceView* CustomMaterialResourceFile::getPreview() const
{
	return nullptr;
}

void CustomMaterialResourceFile::reimport()
{
	MaterialResourceFile::reimport();

	const JSON::json& j = OS::LoadFileContentsToJSONObject(getPath().generic_string());

	m_MaterialData = j;
	MaterialResourceFile::readJSON(j);

	setPixelShader(m_MaterialData.pixelShaderPath);
	m_VSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelVSCBData>(PerModelVSCBData(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

bool CustomMaterialResourceFile::save()
{
	return saveMaterialData(getJSON());
}

void CustomMaterialResourceFile::draw()
{
	MaterialResourceFile::draw();

	if (ImGui::InputTextWithHint("Pixel Shader", "Path to Pixel Shader", &m_MaterialData.pixelShaderPath, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		setPixelShader(m_MaterialData.pixelShaderPath);
	}

	if (ImGui::Button(ICON_ROOTEX_REFRESH "##Rebuild Shader"))
	{
		setPixelShader(m_MaterialData.pixelShaderPath);
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Edit Shader"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, m_MaterialData.pixelShaderPath);
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Find Shader"))
	{
		if (Optional<String> result = OS::SelectFile("Pixel Shader(*.hlsl)\0*.hlsl\0", "game/assets/shaders/"))
		{
			setPixelShader(*result);
		}
	}
	ImGui::SameLine();
	static String newShaderName;
	if (ImGui::Button(ICON_ROOTEX_PLUS "##Create Shader"))
	{
		newShaderName.clear();
		ImGui::OpenPopup("Create Shader");
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_WINDOW_CLOSE "##Reset"))
	{
		setPixelShader(s_DefaultCustomShaderPath);
	}

	if (ImGui::BeginPopup("Create Shader"))
	{
		ImGui::InputTextWithHint("Name", "Pixel Shader Name", &newShaderName);

		String shaderFileName = "game/assets/shaders/" + newShaderName + ".hlsl";
		ImGui::Text("File Name: %s", shaderFileName.c_str());

		if (ImGui::Button("Create"))
		{
			if (OS::IsExists(shaderFileName))
			{
				FileBuffer defaultShaderBuffer = OS::LoadFileContents(s_DefaultCustomShaderPath);
				OS::SaveFile(shaderFileName, defaultShaderBuffer.data(), defaultShaderBuffer.size());

				setPixelShader(shaderFileName);
				EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, shaderFileName);
				newShaderName.clear();
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::ListBoxHeader("Textures"))
	{
		int i = 0;
		for (auto& texture : m_MaterialData.textures)
		{
			String textureName = "Texture " + std::to_string(i) + " " + ICON_ROOTEX_FOLDER_OPEN;
			RootexSelectableImage(textureName.c_str(), texture, [this, i](const String& newTexturePath) { setTexture(newTexturePath, i); });
			i++;
			ImGui::Separator();
		}

		ImGui::ListBoxFooter();
	}

	ImGui::BeginGroup();
	if (ImGui::Button(ICON_ROOTEX_PLUS "##Push Texture"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Image), "game/assets/"))
		{
			if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(*result))
			{
				pushTexture(image);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_MINUS "##Pop Texture"))
	{
		popTexture();
	}
	ImGui::EndGroup();
}
