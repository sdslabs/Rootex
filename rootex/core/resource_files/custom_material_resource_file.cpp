#include "custom_material_resource_file.h"

#include "application.h"
#include "core/renderer/shaders/register_locations_pixel_shader.h"
#include "core/renderer/shaders/register_locations_vertex_shader.h"
#include "framework/systems/render_system.h"
#include "resource_loader.h"
#define MAX_NUMBER_OF_CUSTOM_CB 8
#define MAX_NUMBER_OF_PIXEL_SHADERS 64

void to_json(JSON::json& j, const CustomMaterialData& s)
{
	j["pixelShader"] = s.pixelShaderPath;
	j["pixelShaderCount"] = s.pixelShaderCount;
	j["vertexShader"] = s.vertexShaderPath;
	j["vertexShaderTextures"] = JSON::json::array();
	for (auto& texture : s.vertexShaderTextures)
	{
		j["vertexShaderTextures"].push_back(texture->getPath().generic_string());
	}
	j["pixelShaderTextures"] = JSON::json::array();
	j["pixelShaderTexturesMapping"] = JSON::json::array();
	for (auto& texture : s.pixelShaderTextures)
	{
		j["pixelShaderTextures"].push_back(texture->getPath().generic_string());
	}
	for (auto& index : s.pixelShaderTexturesMapping)
	{
		j["pixelShaderTexturesMapping"].push_back(index);
	}
	for (auto& customConstantBuffers : s.customConstantBuffers)
	{
		j["customConstantBuffers"].push_back(customConstantBuffers);
	}
	for (auto& typeOfCustomConstantBuffers : s.typeOfCustomConstantBuffers)
	{
		j["typeOfCustomConstantBuffers"].push_back(typeOfCustomConstantBuffers);
	}
	j["vertexShaderPath"] = s.dummyVertexShaderPath;
}

void from_json(const JSON::json& j, CustomMaterialData& s)
{
	s.vertexShaderPath = j.value("vertexShader", CustomMaterialResourceFile::s_DefaultCustomVSPath);
	s.pixelShaderPath = j.value("pixelShader", CustomMaterialResourceFile::s_DefaultCustomPSPath);
	s.pixelShaderCount = j.value("pixelShaderCount", 1);
	s.dummyVertexShaderPath = j.value("vertexShaderPath", CustomMaterialResourceFile::s_DefaultCustomVSPath);
	for (auto& texturePath : j.value("vertexShaderTextures", Vector<String>()))
	{
		if (Ref<ImageResourceFile> texture = ResourceLoader::CreateImageResourceFile(texturePath))
		{
			s.vertexShaderTextures.push_back(texture);
		}
	}
	for (auto& texturePath : j.value("pixelShaderTextures", Vector<String>({ "rootex/assets/rootex.png" })))
	{
		if (Ref<ImageResourceFile> texture = ResourceLoader::CreateImageResourceFile(texturePath))
		{
			s.pixelShaderTextures.push_back(texture);
		}
	}
	for (auto& index : j.value("pixelShaderTexturesMapping", Vector<int>()))
	{
		s.pixelShaderTexturesMapping.push_back(index);
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
	m_PixelShaderCount = 0;
	m_DummyVertexShaderPath = "rootex/core/renderer/shaders/custom_vertex_shader.hlsl";
	reimport();
}

void CustomMaterialResourceFile::pushPSTexture(Ref<ImageResourceFile> texture, int ind)
{
	if (!texture)
	{
		WARN("Skipping null PS texture which was tried to be pushed");
		return;
	}
	int a;
	a = m_MaterialData.pixelShaderTextures.size();
	if ((ind + 1) < m_MaterialData.pixelShaderTexturesMapping.size())
		a = m_MaterialData.pixelShaderTexturesMapping[ind + 1];
	Ref<ImageResourceFile> nextTexture;
	for (int i = a; i < m_MaterialData.pixelShaderTextures.size(); i++)
	{
		nextTexture = m_MaterialData.pixelShaderTextures[i];
		m_MaterialData.pixelShaderTextures[i] = texture;
		texture = nextTexture;
	}
	for (int i = ind + 1; i < m_MaterialData.pixelShaderTexturesMapping.size(); i++)
	{
		m_MaterialData.pixelShaderTexturesMapping[i] = m_MaterialData.pixelShaderTexturesMapping[i] + 1;
	}
	m_MaterialData.pixelShaderTextures.push_back(texture);
}

void CustomMaterialResourceFile::setPSTexture(const String& newtexturePath, int position, int ind)
{
	if (position >= m_MaterialData.pixelShaderTextures.size() || position < 0)
	{
		WARN("PS position being set is out of bounds: " + std::to_string(position));
		return;
	}

	int a = m_MaterialData.pixelShaderTexturesMapping[ind];
	if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(newtexturePath))
	{
		m_MaterialData.pixelShaderTextures[a + position] = image;
	}
}

void CustomMaterialResourceFile::popPSTexture(int ind)
{
	if (m_MaterialData.pixelShaderTextures.empty())
	{
		WARN("PS texture list is empty.");
		return;
	}
	bool bl = false;
	if ((ind + 1) < m_MaterialData.pixelShaderTexturesMapping.size())
		bl = true;
	int a;
	if (bl)
		a = m_MaterialData.pixelShaderTexturesMapping[ind + 1];
	else
		a = m_MaterialData.pixelShaderTextures.size();
	int j = a;
	a--;
	Ref<ImageResourceFile> nextTexture, texture;
	for (int i = a; i < m_MaterialData.pixelShaderTextures.size() - 1; i++)
	{
		nextTexture = m_MaterialData.pixelShaderTextures[i + 1];
		m_MaterialData.pixelShaderTextures[i] = nextTexture;
	}
	for (int i = ind + 1; i < m_MaterialData.pixelShaderTexturesMapping.size(); i++)
	{
		m_MaterialData.pixelShaderTexturesMapping[i]--;
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
	int j = m_MaterialData.pixelShaderTexturesMapping[index];
	int a = m_MaterialData.pixelShaderTexturesMapping[index];
	if (a >= m_MaterialData.pixelShaderTexturesMapping.size())
		a = m_MaterialData.pixelShaderTexturesMapping.size();
	for (; j < a;)
	{
		auto imageRes = m_MaterialData.pixelShaderTextures[j];
		textures.push_back(imageRes->getGPUTexture());
		j++;
	}

	return textures;
}

void CustomMaterialResourceFile::setShaders(int ind, const String& vertexShader, const String& pixelShader)
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
		if (ind >= m_Shader.size())
			m_Shader.emplace_back(std::move(shader));
		else
			m_Shader[ind] = std::move(shader);
	}
}

void CustomMaterialResourceFile::setVS(int ind, const String& vertexShader)
{
	setShaders(ind, vertexShader, m_MaterialData.pixelShaderPath);
}

void CustomMaterialResourceFile::setPS(int ind, const String& pixelShader)
{
	if (ind == 0)
		setShaders(ind, m_MaterialData.vertexShaderPath, pixelShader);
	else
		setShaders(ind, m_DummyVertexShaderPath, pixelShader);
}

void CustomMaterialResourceFile::recompileShaders(int ind)
{
	setShaders(ind, m_MaterialData.vertexShaderPath, m_MaterialData.pixelShaderPath);
}

void CustomMaterialResourceFile::bindShader()
{
	int k = 0;
	m_Shader[index]->bind();
	k = 1;
}

void CustomMaterialResourceFile::bindTextures()
{
	int a = m_MaterialData.pixelShaderTexturesMapping[index];
	if (m_MaterialData.pixelShaderTextures.size() <= a)
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

	for (int i = 0; i < m_PixelShaderCount; i++)
	{
		recompileShaders(i);
	}
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
		for (int ind = 0; ind < m_PixelShaderCount; ind++)
			recompileShaders(ind);
	}

	if (ImGui::TreeNode("Vertex Shader"))
	{
		if (ImGui::InputTextWithHint("Path", "Path to Vertex Shader", &m_MaterialData.vertexShaderPath, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			for (int ind = 0; ind < m_PixelShaderCount; ind++)
			{
				if (ind == 0)
					setVS(ind, m_MaterialData.vertexShaderPath);
				else
					setVS(ind, m_DummyVertexShaderPath);
			}
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
				for (int ind = 0; ind < m_PixelShaderCount; ind++)
				{
					if (ind == 0)
						setVS(ind, *result);
					else
						setVS(ind, m_DummyVertexShaderPath);
				}
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
			for (int ind = 0; ind < m_PixelShaderCount; ind++)
			{
				if (ind == 0)
					setVS(ind, s_DefaultCustomVSPath);
				else
					setVS(ind, m_DummyVertexShaderPath);
			}
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

				for (int ind = 0; ind < m_PixelShaderCount; ind++)
				{
					if (ind == 0)
						setVS(ind, shaderFileName);
					else
						setVS(ind, m_DummyVertexShaderPath);
				}
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
		int ind = 0;
		String s1, s2, s3, s4;
		for (; ind < m_PixelShaderCount; ind++)
		{
			index = ind;
			ImGui::PushID(ind);
			s1 = "Path" + std::to_string(ind);
			bool bl = false;
			if (ind >= m_Shader.size())
				setPS(ind, s_DefaultCustomPSPath);
			if (ImGui::InputTextWithHint(s1.c_str(), "Path to Pixel Shader", &m_MaterialData.pixelShaderPath, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				setPS(ind, m_MaterialData.pixelShaderPath);
			}
			s2 = "Create PS" + std::to_string(ind);
			if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Edit PS"))
			{
				EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, m_MaterialData.pixelShaderPath);
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Find PS"))
			{
				if (Optional<String> result = OS::SelectFile("Pixel Shader(*.hlsl)\0*.hlsl\0", "game/assets/shaders/"))
				{
					setPS(ind, *result);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_ROOTEX_PLUS "##Create PS"))
			{
				ImGui::OpenPopup(s2.c_str(), true);
				bl = true;
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_ROOTEX_WINDOW_CLOSE "##Reset PS"))
			{
				setPS(ind, s_DefaultCustomPSPath);
			}

			if (ImGui::ListBoxHeader("Textures##PS"))
			{
				int i = 0;
				int a = m_MaterialData.pixelShaderTexturesMapping[ind];
				int b = m_MaterialData.pixelShaderTextures.size();
				if (ind < (m_PixelShaderCount - 1))
					b = m_MaterialData.pixelShaderTexturesMapping[ind + 1];
				for (i = a; i < b; i++)
				{
					Ref<ImageResourceFile> texture = m_MaterialData.pixelShaderTextures[i];
					String textureName = "Slot " + std::to_string((i)) + " " + ICON_ROOTEX_FOLDER_OPEN;
					RootexSelectableImage(textureName.c_str(), texture, [this, i, ind, a](const String& newTexturePath) { setPSTexture(newTexturePath, i - a, ind); });
					ImGui::Separator();
				}

				if (ImGui::Button(ICON_ROOTEX_PLUS "##Push PS"))
				{
					if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Image), "game/assets/"))
					{
						if (Ref<ImageResourceFile> image = ResourceLoader::CreateImageResourceFile(*result))
						{
							pushPSTexture(image, ind);
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button(ICON_ROOTEX_MINUS "##Pop PS"))
				{
					popPSTexture(ind);
				}

				ImGui::ListBoxFooter();
			}
			if (ImGui::Button("Remove This Shader"))
			{
				if (m_PixelShaderCount > 0)
				{
					int i = 0;
					auto itr = m_Shader.begin();
					while (i != ind)
					{
						itr++;
						i++;
					}
					int a = m_MaterialData.pixelShaderTexturesMapping[ind];
					int b = m_MaterialData.pixelShaderTextures.size();

					if (ind + 1 < m_PixelShaderCount)
						b = m_MaterialData.pixelShaderTexturesMapping[ind + 1];
					for (int k = a; k < b; k++)
					{
						popPSTexture(ind);
					}
					for (int j = ind; j < m_MaterialData.pixelShaderTexturesMapping.size() - 1; j++)
					{
						m_MaterialData.pixelShaderTexturesMapping[j] = m_MaterialData.pixelShaderTexturesMapping[j + 1];
					}
					m_MaterialData.pixelShaderTexturesMapping.pop_back();
					if (m_Shader.size() > ind)
					{
						m_Shader.erase(itr);
						m_PixelShaderCount--;
					}
				}
				else
				{
					ERROR("No pixel shaders left to be removed");
				}
			}
			ImGui::PopID();
			ImGui::Separator();
		}

		if (ImGui::Button("Add Pixel Shader"))
		{
			if (m_PixelShaderCount < MAX_NUMBER_OF_PIXEL_SHADERS)
			{
				m_PixelShaderCount++;
				m_MaterialData.pixelShaderTexturesMapping.push_back(m_MaterialData.pixelShaderTextures.size());
			}
			else
			{
				ERROR("Exceeded maximum pixel shader count");
			}
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

				for (int ind = 0; ind < m_PixelShaderCount; ind++)
					setPS(ind, shaderFileName);
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
	m_MaterialData.dummyVertexShaderPath = m_DummyVertexShaderPath;
	m_MaterialData.pixelShaderCount = m_PixelShaderCount;
}
