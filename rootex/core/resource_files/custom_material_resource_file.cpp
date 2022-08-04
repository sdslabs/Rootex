#include "custom_material_resource_file.h"

#include "application.h"
#include "core/renderer/shaders/register_locations_pixel_shader.h"
#include "core/renderer/shaders/register_locations_vertex_shader.h"
#include "framework/systems/render_system.h"
#include "resource_loader.h"
#define MAX_NUMBER_OF_CUSTOM_CB 8

void to_json(JSON::json& j, const CustomMaterialData& s)
{
	j["pixelShader"] = s.pixelShaderPath;
	j["pixelShaderTextures"] = JSON::json::array();
	for (auto& texture : s.pixelShaderTextures)
	{
		j["pixelShaderTextures"].push_back(texture->getPath().generic_string());
	}
	j["vertexShader"] = s.vertexShaderPath;
	j["vertexShaderTextures"] = JSON::json::array();
	for (auto& texture : s.vertexShaderTextures)
	{
		j["vertexShaderTextures"].push_back(texture->getPath().generic_string());
	}
	for (auto& customConstantBuffers : s.customConstantBuffers)
	{
		j["customConstantBuffers"].push_back(customConstantBuffers);
	}
	for (auto& typeOfCustomConstantBuffers : s.typeOfCustomConstantBuffers)
	{
		j["typeOfCustomConstantBuffers"].push_back(typeOfCustomConstantBuffers);
	}
}

void from_json(const JSON::json& j, CustomMaterialData& s)
{
	s.vertexShaderPath = j.value("vertexShader", CustomMaterialResourceFile::s_DefaultCustomVSPath);
	s.pixelShaderPath = j.value("pixelShader", CustomMaterialResourceFile::s_DefaultCustomPSPath);
	for (auto& texturePath : j.value("pixelShaderTextures", Vector<String>({ "rootex/assets/rootex.png" })))
	{
		if (Ref<ImageResourceFile> texture = ResourceLoader::CreateImageResourceFile(texturePath))
		{
			s.pixelShaderTextures.push_back(texture);
		}
	}
	for (auto& texturePath : j.value("vertexShaderTextures", Vector<String>()))
	{
		if (Ref<ImageResourceFile> texture = ResourceLoader::CreateImageResourceFile(texturePath))
		{
			s.vertexShaderTextures.push_back(texture);
		}
	}
	for (auto& customConstantBuffers : j.value("customConstantBuffers", Vector<float>()))
	{
		s.customConstantBuffers.push_back(customConstantBuffers);
	}
	for (auto& typeOfCustomConstantBuffers : j.value("typeOfCustomConstantBuffers", Vector<TYPES_OF_BUFFERS>()))
	{
		s.typeOfCustomConstantBuffers.push_back(typeOfCustomConstantBuffers);
	}
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

void CustomMaterialResourceFile::pushPSTexture(Ref<ImageResourceFile> texture)
{
	if (!texture)
	{
		WARN("Skipping null PS texture which was tried to be pushed");
		return;
	}

	m_MaterialData.pixelShaderTextures.push_back(texture);
}

void CustomMaterialResourceFile::setPSTexture(const String& newtexturePath, int position)
{
	if (position >= m_MaterialData.pixelShaderTextures.size() || position < 0)
	{
		WARN("PS position being set is out of bounds: " + std::to_string(position));
		return;
	}

	if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(newtexturePath))
	{
		m_MaterialData.pixelShaderTextures[position] = image;
	}
}

void CustomMaterialResourceFile::popPSTexture()
{
	if (m_MaterialData.pixelShaderTextures.empty())
	{
		WARN("PS texture list is empty.");
		return;
	}

	m_MaterialData.pixelShaderTextures.pop_back();
}

void CustomMaterialResourceFile::pushVSTexture(Ref<ImageResourceFile> texture)
{
	if (!texture)
	{
		WARN("Skipping null VS texture which was tried to be pushed");
		return;
	}

	m_MaterialData.vertexShaderTextures.push_back(texture);
}

void CustomMaterialResourceFile::setVSTexture(const String& newtexturePath, int position)
{
	if (position >= m_MaterialData.vertexShaderTextures.size() || position < 0)
	{
		WARN("VS position being set is out of bounds: " + std::to_string(position));
		return;
	}

	if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(newtexturePath))
	{
		m_MaterialData.vertexShaderTextures[position] = image;
	}
}

void CustomMaterialResourceFile::popVSTexture()
{
	if (m_MaterialData.vertexShaderTextures.empty())
	{
		WARN("VS Texture list is empty.");
		return;
	}

	m_MaterialData.vertexShaderTextures.pop_back();
}

Vector<Ref<GPUTexture>> CustomMaterialResourceFile::getTextures() const
{
	Vector<Ref<GPUTexture>> textures;
	for (auto& imageRes : m_MaterialData.vertexShaderTextures)
	{
		textures.push_back(imageRes->getGPUTexture());
	}
	for (auto& imageRes : m_MaterialData.pixelShaderTextures)
	{
		textures.push_back(imageRes->getGPUTexture());
	}

	return textures;
}

void CustomMaterialResourceFile::setShaders(const String& vertexShader, const String& pixelShader)
{
	if (!OS::IsExists(pixelShader) || !OS::IsExists(vertexShader))
	{
		ERR("Custom shaders not found: " + vertexShader + " or " + pixelShader);
		return;
	}

	BufferFormat bufferFormat;
	bufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	bufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "NORMAL", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	bufferFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	bufferFormat.push(VertexBufferElement::Type::FloatFloatFloat, "TANGENT", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	Ptr<Shader> shader(new Shader(vertexShader, pixelShader, bufferFormat));

	if (shader->isValid())
	{
		m_MaterialData.pixelShaderPath = pixelShader;
		m_MaterialData.vertexShaderPath = vertexShader;
		m_Shader = std::move(shader);
	}
}

void CustomMaterialResourceFile::setVS(const String& vertexShader)
{
	setShaders(vertexShader, m_MaterialData.pixelShaderPath);
}

void CustomMaterialResourceFile::setPS(const String& pixelShader)
{
	setShaders(m_MaterialData.vertexShaderPath, pixelShader);
}

void CustomMaterialResourceFile::recompileShaders()
{
	setShaders(m_MaterialData.vertexShaderPath, m_MaterialData.pixelShaderPath);
}

void CustomMaterialResourceFile::bindShader()
{
	m_Shader->bind();
}

void CustomMaterialResourceFile::bindTextures()
{
	if (!m_MaterialData.pixelShaderTextures.empty())
	{
		static Vector<ID3D11ShaderResourceView*> textureSRVs;
		textureSRVs.clear();
		for (auto& texture : m_MaterialData.pixelShaderTextures)
		{
			textureSRVs.push_back(texture->getGPUTexture()->getTextureResourceView());
		}

		RenderingDevice::GetSingleton()->setPSSRV(CUSTOM_TEXTURE_0_PS_CPP, textureSRVs.size(), textureSRVs.data());
	}

	if (!m_MaterialData.vertexShaderTextures.empty())
	{
		static Vector<ID3D11ShaderResourceView*> textureSRVs;
		textureSRVs.clear();
		for (auto& texture : m_MaterialData.vertexShaderTextures)
		{
			textureSRVs.push_back(texture->getGPUTexture()->getTextureResourceView());
		}

		RenderingDevice::GetSingleton()->setVSSRV(CUSTOM_TEXTURE_0_VS_CPP, textureSRVs.size(), textureSRVs.data());
	}
}

void CustomMaterialResourceFile::bindSamplers()
{
	RenderingDevice::GetSingleton()->setPSSS(SAMPLER_PS_CPP, 1, s_Sampler.GetAddressOf());
}

void CustomMaterialResourceFile::bindVSCB()
{
	RenderingDevice::GetSingleton()->editBuffer(PerModelVSCBData(TransformSystem::GetSingleton()->getCurrentMatrix()), m_VSCB.Get());
	RenderingDevice::GetSingleton()->setVSCB(PER_OBJECT_VS_CPP, 1, m_VSCB.GetAddressOf());
}

void CustomMaterialResourceFile::bindPSCB()
{
	int size = customConstantBuffers.size() * sizeof(float);
	RenderingDevice::GetSingleton()->editBuffer((const char*)customConstantBuffers.data(), size, m_PSCB.Get());
	RenderingDevice::GetSingleton()->setPSCB(CUSTOM_PER_OBJECT_PS_CPP, 1, m_PSCB.GetAddressOf());
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

	customConstantBuffers = m_MaterialData.customConstantBuffers;
	typeOfCustomConstantBuffers = m_MaterialData.typeOfCustomConstantBuffers;

	recompileShaders();
	float fakeArray[MAX_NUMBER_OF_CUSTOM_CB * 4];
	m_PSCB = RenderingDevice::GetSingleton()->createBuffer((const char*)fakeArray, sizeof(fakeArray), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	m_VSCB = RenderingDevice::GetSingleton()->createBuffer<PerModelVSCBData>(PerModelVSCBData(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

bool CustomMaterialResourceFile::save()
{
	return saveMaterialData(getJSON());
}

float CustomMaterialResourceFile::getFloat(int index)
{
	if (4 * index < customConstantBuffers.size())
		return customConstantBuffers[4 * index];
	return 0.0f;
}

Vector3 CustomMaterialResourceFile::getFloat3(int index)
{
	Vector3 temp = { 0.0f, 0.0f, 0.0f };
	if (4 * index < customConstantBuffers.size())
	{
		temp.x = customConstantBuffers[4 * index];
		temp.y = customConstantBuffers[4 * index + 1];
		temp.z = customConstantBuffers[4 * index + 2];
	}
	return temp;
}

Color CustomMaterialResourceFile::getColor(int index)
{
	Color temp = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (4 * index < customConstantBuffers.size())
	{
		temp.x = customConstantBuffers[4 * index];
		temp.y = customConstantBuffers[4 * index + 1];
		temp.z = customConstantBuffers[4 * index + 2];
		temp.w = customConstantBuffers[4 * index + 3];
	}
	return temp;
}

bool CustomMaterialResourceFile::setFloat(int index, float value)
{
	if (4 * index < customConstantBuffers.size())
	{
		customConstantBuffers[4 * index] = value;
		return true;
	}
	return false;
}

bool CustomMaterialResourceFile::setFloat3(int index, Vector3 value)
{
	if (4 * index < customConstantBuffers.size())
	{
		customConstantBuffers[4 * index] = value.x;
		customConstantBuffers[4 * index + 1] = value.y;
		customConstantBuffers[4 * index + 2] = value.z;
		return true;
	}
	return false;
}

bool CustomMaterialResourceFile::setColor(int index, Color value)
{
	if (4 * index < customConstantBuffers.size())
	{
		customConstantBuffers[4 * index] = value.x;
		customConstantBuffers[4 * index + 1] = value.y;
		customConstantBuffers[4 * index + 2] = value.z;
		customConstantBuffers[4 * index + 3] = value.w;
		return true;
	}
	return false;
}

void CustomMaterialResourceFile::draw()
{
	MaterialResourceFile::draw();

	if (ImGui::Button("Recompile " ICON_ROOTEX_REFRESH))
	{
		recompileShaders();
	}

	if (ImGui::TreeNode("Vertex Shader"))
	{
		if (ImGui::InputTextWithHint("Path", "Path to Vertex Shader", &m_MaterialData.vertexShaderPath, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			setVS(m_MaterialData.vertexShaderPath);
		}

		if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Edit VS"))
		{
			EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, m_MaterialData.vertexShaderPath);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Find VS"))
		{
			if (Optional<String> result = OS::SelectFile("Vertex Shader(*.hlsl)\0*.hlsl\0", "game/assets/shaders/"))
			{
				setVS(*result);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_PLUS "##Create VS"))
		{
			ImGui::OpenPopup("Create VS");
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_WINDOW_CLOSE "##Reset VS"))
		{
			setVS(s_DefaultCustomVSPath);
		}

		if (ImGui::ListBoxHeader("Textures##VS"))
		{
			int i = 0;
			for (auto& texture : m_MaterialData.vertexShaderTextures)
			{
				String textureName = "Slot " + std::to_string(i) + " " + ICON_ROOTEX_FOLDER_OPEN;
				RootexSelectableImage(textureName.c_str(), texture, [this, i](const String& newTexturePath) { setVSTexture(newTexturePath, i); });
				i++;
				ImGui::Separator();
			}

			if (ImGui::Button(ICON_ROOTEX_PLUS "##Push VS"))
			{
				if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Image), "game/assets/"))
				{
					if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(*result))
					{
						pushVSTexture(image);
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_ROOTEX_MINUS "##Pop VS"))
			{
				popVSTexture();
			}

			ImGui::ListBoxFooter();
		}

		if (ImGui::BeginPopup("Create VS"))
		{
			static String newVSPath;
			ImGui::InputTextWithHint("Name", "VS Name", &newVSPath);

			String shaderFileName = "game/assets/shaders/" + newVSPath + "_vertex_shader.hlsl";
			ImGui::Text("File Name: %s", shaderFileName.c_str());

			if (!newVSPath.empty() && !OS::IsExists(shaderFileName) && ImGui::Button("Create##VS"))
			{
				FileBuffer defaultShaderBuffer = OS::LoadFileContents(s_DefaultCustomVSPath);
				OS::SaveFile(shaderFileName, defaultShaderBuffer.data(), defaultShaderBuffer.size());

				setVS(shaderFileName);
				EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, shaderFileName);
				newVSPath.clear();

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Pixel Shader"))
	{
		if (ImGui::InputTextWithHint("Path", "Path to Pixel Shader", &m_MaterialData.pixelShaderPath, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			setPS(m_MaterialData.pixelShaderPath);
		}

		if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Edit PS"))
		{
			EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, m_MaterialData.pixelShaderPath);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Find PS"))
		{
			if (Optional<String> result = OS::SelectFile("Pixel Shader(*.hlsl)\0*.hlsl\0", "game/assets/shaders/"))
			{
				setPS(*result);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_PLUS "##Create PS"))
		{
			ImGui::OpenPopup("Create PS");
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_WINDOW_CLOSE "##Reset PS"))
		{
			setPS(s_DefaultCustomPSPath);
		}

		if (ImGui::ListBoxHeader("Textures##PS"))
		{
			int i = 0;
			for (auto& texture : m_MaterialData.pixelShaderTextures)
			{
				String textureName = "Slot " + std::to_string(i) + " " + ICON_ROOTEX_FOLDER_OPEN;
				RootexSelectableImage(textureName.c_str(), texture, [this, i](const String& newTexturePath) { setPSTexture(newTexturePath, i); });
				i++;
				ImGui::Separator();
			}

			if (ImGui::Button(ICON_ROOTEX_PLUS "##Push PS"))
			{
				if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Image), "game/assets/"))
				{
					if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(*result))
					{
						pushPSTexture(image);
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_ROOTEX_MINUS "##Pop PS"))
			{
				popPSTexture();
			}

			ImGui::ListBoxFooter();
		}

		if (ImGui::BeginPopup("Create PS"))
		{
			static String newPSPath;
			ImGui::InputTextWithHint("Name", "PS Name", &newPSPath);

			String shaderFileName = "game/assets/shaders/" + newPSPath + "_pixel_shader.hlsl";
			ImGui::Text("File Name: %s", shaderFileName.c_str());

			if (!newPSPath.empty() && !OS::IsExists(shaderFileName) && ImGui::Button("Create##PS"))
			{
				FileBuffer defaultShaderBuffer = OS::LoadFileContents(s_DefaultCustomPSPath);
				OS::SaveFile(shaderFileName, defaultShaderBuffer.data(), defaultShaderBuffer.size());

				setPS(shaderFileName);
				EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, shaderFileName);
				newPSPath.clear();

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::TreePop();
	}

	for (int i = 0; i < customConstantBuffers.size(); i += 4)
	{
		String customConstantBufferName = "CB Slot " + std::to_string(i / 4);
		switch (typeOfCustomConstantBuffers[i / 4])
		{
		case TYPES_OF_BUFFERS::FLOATCB:
			ImGui::DragFloat(customConstantBufferName.c_str(), &customConstantBuffers[i], 0.01f, 0.0f, 10.0f);
			break;
		case TYPES_OF_BUFFERS::FLOAT3CB:
			ImGui::DragFloat3(customConstantBufferName.c_str(), &customConstantBuffers[i], 0.01f, 0.0f, 10.0f);
			break;
		case TYPES_OF_BUFFERS::COLORCB:
			ImGui::ColorPicker4(customConstantBufferName.c_str(), &customConstantBuffers[i]);
			break;
		}
		ImGui::Separator();
	}

	if (customConstantBuffers.size() < MAX_NUMBER_OF_CUSTOM_CB * sizeof(float))
	{
		if (ImGui::Button(ICON_ROOTEX_PLUS "Push float CB"))
		{
			float value = 1.0;
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			typeOfCustomConstantBuffers.push_back(TYPES_OF_BUFFERS::FLOATCB);
		}
		ImGui::SameLine();

		if (ImGui::Button(ICON_ROOTEX_PLUS "Push float3 CB"))
		{
			float value = 1.0;
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			typeOfCustomConstantBuffers.push_back(TYPES_OF_BUFFERS::FLOAT3CB);
		}
		ImGui::SameLine();

		if (ImGui::Button(ICON_ROOTEX_PLUS "Push Color CB"))
		{
			float value = 1.0;
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			customConstantBuffers.push_back(value);
			typeOfCustomConstantBuffers.push_back(TYPES_OF_BUFFERS::COLORCB);
		}
		ImGui::SameLine();
	}
	if (ImGui::Button(ICON_ROOTEX_MINUS "Pop CB"))
	{
		customConstantBuffers.pop_back();
		customConstantBuffers.pop_back();
		customConstantBuffers.pop_back();
		customConstantBuffers.pop_back();
		typeOfCustomConstantBuffers.pop_back();
	}
	m_MaterialData.customConstantBuffers = customConstantBuffers;
	m_MaterialData.typeOfCustomConstantBuffers = typeOfCustomConstantBuffers;
}
