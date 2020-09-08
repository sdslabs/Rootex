#include "resource_loader.h"

#include <d3d11.h>

#include "common/common.h"
#include "application.h"
#include "framework/systems/audio_system.h"
#include "core/renderer/mesh.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/material.h"
#include "core/renderer/vertex_data.h"
#include "script/interpreter.h"
#include "core/renderer/material_library.h"
#include "os/thread.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> ResourceLoader::s_ResourcesDataFiles;

bool IsFileSupported(const String& extension, ResourceFile::Type supportedFileType)
{
	auto& findIt = SupportedFiles.find(supportedFileType);
	if (findIt != SupportedFiles.end())
	{
		auto& findExtensionIt = std::find(findIt->second.begin(), findIt->second.end(), extension);
		return findExtensionIt != findIt->second.end();
	}
	return false;
}

void ResourceLoader::LoadAssimp(ModelResourceFile* file)
{
	Assimp::Importer modelLoader;
	const aiScene* scene = modelLoader.ReadFile(
	    file->getPath().generic_string(),
	    aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);

	if (!scene)
	{
		ERR("Model could not be loaded: " + OS::GetAbsolutePath(file->m_ResourceData->getPath().string()).generic_string());
		ERR("Assimp: " + modelLoader.GetErrorString());
		return;
	}

	Vector<Ref<Texture>> textures;
	textures.resize(scene->mNumTextures, nullptr);
	file->m_Meshes.clear();
	file->m_Meshes.reserve(scene->mNumMeshes);
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];

		Vector<VertexData> vertices;
		vertices.reserve(mesh->mNumVertices);

		VertexData vertex;
		ZeroMemory(&vertex, sizeof(VertexData));
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			vertex.m_Position.x = mesh->mVertices[v].x;
			vertex.m_Position.y = mesh->mVertices[v].y;
			vertex.m_Position.z = mesh->mVertices[v].z;

			if (mesh->mNormals)
			{
				vertex.m_Normal.x = mesh->mNormals[v].x;
				vertex.m_Normal.y = mesh->mNormals[v].y;
				vertex.m_Normal.z = mesh->mNormals[v].z;
			}

			if (mesh->mTextureCoords)
			{
				if (mesh->mTextureCoords[0])
				{
					// Assuming the model has texture coordinates and taking only the first texture coordinate in case of multiple texture coordinates
					vertex.m_TextureCoord.x = mesh->mTextureCoords[0][v].x;
					vertex.m_TextureCoord.y = mesh->mTextureCoords[0][v].y;
				}
			}

			vertices.push_back(vertex);
		}

		std::vector<unsigned short> indices;

		aiFace* face = nullptr;
		for (unsigned int f = 0; f < mesh->mNumFaces; f++)
		{
			face = &mesh->mFaces[f];
			//Model already triangulated by aiProcess_Triangulate so no need to check
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiColor3D color(0.0f, 0.0f, 0.0f);
		float alpha = 1.0f;
		if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
			WARN("Material does not have color: " + String(material->GetName().C_Str()));
		}
		if (AI_SUCCESS != material->Get(AI_MATKEY_OPACITY, alpha))
		{
			WARN("Material does not have alpha: " + String(material->GetName().C_Str()));
		}

		Ref<BasicMaterial> extractedMaterial;
		
		String materialPath;
		if (String(material->GetName().C_Str()) == "DefaultMaterial")
		{
			materialPath = "rootex/assets/materials/default.rmat";
		}
		else
		{
			materialPath = "game/assets/materials/" + String(material->GetName().C_Str()) + ".rmat";
		}

		if (MaterialLibrary::IsExists(materialPath))
		{
			extractedMaterial = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial(materialPath));
		}
		else
		{
			MaterialLibrary::CreateNewMaterialFile(materialPath, "BasicMaterial");
			extractedMaterial = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial(materialPath));
			extractedMaterial->setColor({ color.r, color.g, color.b, alpha });

			for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE, i, &str);
				char embeddedAsterisk = *str.C_Str();

				if (embeddedAsterisk == '*')
				{
					// Texture is embedded
					int textureID = atoi(str.C_Str() + 1);

					if (!textures[textureID])
					{
						aiTexture* texture = scene->mTextures[textureID];
						size_t size = scene->mTextures[textureID]->mWidth;
						PANIC(texture->mHeight == 0, "Compressed texture found but expected embedded texture");
						textures[textureID].reset(new Texture(reinterpret_cast<const char*>(texture->pcData), size));
					}

					extractedMaterial->setTextureInternal(textures[textureID]);
				}
				else
				{
					// Texture is given as a path
					String texturePath = str.C_Str();
					ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(file->getPath().parent_path().generic_string() + "/" + texturePath);

					if (image)
					{
						extractedMaterial->setTexture(image);
					}
					else
					{
						WARN("Could not set material diffuse texture: " + texturePath);
					}
				}
			}
		}

		Mesh extractedMesh;
		extractedMesh.m_VertexBuffer.reset(new VertexBuffer(vertices));
		extractedMesh.m_IndexBuffer.reset(new IndexBuffer(indices));
		
		file->m_Meshes[extractedMaterial].push_back(extractedMesh);
	}
}

void ResourceLoader::LoadALUT(AudioResourceFile* audioRes, const char* audioBuffer, int format, int size, float frequency)
{
	audioRes->m_DecompressedAudioBuffer = audioBuffer;
	audioRes->m_Format = format;
	audioRes->m_AudioDataSize = size;
	audioRes->m_Frequency = frequency;

	switch (audioRes->getFormat())
	{
	case AL_FORMAT_MONO8:
		audioRes->m_Channels = 1;
		audioRes->m_BitDepth = 8;
		break;

	case AL_FORMAT_MONO16:
		audioRes->m_Channels = 1;
		audioRes->m_BitDepth = 16;
		break;

	case AL_FORMAT_STEREO8:
		audioRes->m_Channels = 2;
		audioRes->m_BitDepth = 8;
		break;

	case AL_FORMAT_STEREO16:
		audioRes->m_Channels = 2;
		audioRes->m_BitDepth = 16;
		break;

	default:
		ERR("Unknown channels and bit depth in WAV data");
	}

	audioRes->m_Duration = size * 8 / (audioRes->m_Channels * audioRes->m_BitDepth);
	audioRes->m_Duration /= frequency;
}

ResourceFile* ResourceLoader::CreateSomeResourceFile(const String& path)
{
	ResourceFile* result = nullptr;
	for (auto& [resourceType, extensions] : SupportedFiles)
	{
		if (std::find(extensions.begin(), extensions.end(), FilePath(path).extension().generic_string()) != extensions.end())
		{
			switch (resourceType)
			{
			case ResourceFile::Type::Text:
				result = CreateTextResourceFile(path);
				break;
			case ResourceFile::Type::Audio:
				result = CreateAudioResourceFile(path);
				break;
			case ResourceFile::Type::Font:
				result = CreateFontResourceFile(path);
				break;
			case ResourceFile::Type::Image:
				result = CreateImageResourceFile(path);
				break;
			case ResourceFile::Type::Lua:
				result = CreateLuaTextResourceFile(path);
				break;
			case ResourceFile::Type::Model:
				result = CreateModelResourceFile(path);
				break;
			default:
				break;
			}
		}
	}
	return result;
}

void ResourceLoader::RegisterAPI(sol::state& rootex)
{
	sol::usertype<ResourceLoader> resourceLoader = rootex.new_usertype<ResourceLoader>("ResourceLoader");
	resourceLoader["CreateAudio"] = &ResourceLoader::CreateAudioResourceFile;
	resourceLoader["CreateFont"] = &ResourceLoader::CreateFontResourceFile;
	resourceLoader["CreateImage"] = &ResourceLoader::CreateImageResourceFile;
	resourceLoader["CreateLua"] = &ResourceLoader::CreateLuaTextResourceFile;
	resourceLoader["CreateText"] = &ResourceLoader::CreateTextResourceFile;
	resourceLoader["CreateNewText"] = &ResourceLoader::CreateNewTextResourceFile;
	resourceLoader["CreateVisualModel"] = &ResourceLoader::CreateModelResourceFile;
}

TextResourceFile* ResourceLoader::CreateTextResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Text)
		{
			return reinterpret_cast<TextResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	TextResourceFile* textRes = new TextResourceFile(ResourceFile::Type::Text, resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(textRes);
	return textRes;
}

TextResourceFile* ResourceLoader::CreateNewTextResourceFile(const String& path)
{
	if (!OS::IsExists(path))
	{
		OS::CreateFileName(path);
	}
	return CreateTextResourceFile(path);
}

LuaTextResourceFile* ResourceLoader::CreateLuaTextResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Lua)
		{
			return reinterpret_cast<LuaTextResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	LuaTextResourceFile* luaRes = new LuaTextResourceFile(resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(luaRes);

	return luaRes;
}

AudioResourceFile* ResourceLoader::CreateAudioResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Audio)
		{
			return reinterpret_cast<AudioResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	const char* audioBuffer;
	int format;
	int size;
	float frequency;
	ALUT_CHECK(audioBuffer = (const char*)alutLoadMemoryFromFile(
	               OS::GetAbsolutePath(path).generic_string().c_str(),
	               &format,
	               &size,
	               &frequency));

	Vector<char> dataArray;
	dataArray.insert(
	    dataArray.begin(),
	    audioBuffer,
	    audioBuffer + size);
	ResourceData* resData = new ResourceData(path, dataArray);

	AudioResourceFile* audioRes = new AudioResourceFile(resData);
	LoadALUT(audioRes, audioBuffer, format, size, frequency);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(audioRes);

	return audioRes;
}

ModelResourceFile* ResourceLoader::CreateModelResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Model)
		{
			return reinterpret_cast<ModelResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}
	
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	ModelResourceFile* visualRes = new ModelResourceFile(resData);
	
	LoadAssimp(visualRes);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(visualRes);

	return visualRes;
}

ImageResourceFile* ResourceLoader::CreateImageResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Image)
		{
			return reinterpret_cast<ImageResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	ImageResourceFile* imageRes = new ImageResourceFile(resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(imageRes);

	return imageRes;
}

FontResourceFile* ResourceLoader::CreateFontResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Font)
		{
			return reinterpret_cast<FontResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	FontResourceFile* fontRes = new FontResourceFile(resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(fontRes);

	return fontRes;
}

void ResourceLoader::SaveResourceFile(ResourceFile* resourceFile)
{
	bool saved = OS::SaveFile(resourceFile->getPath(), resourceFile->getData());
	PANIC(saved == false, "Old resource could not be located for saving file: " + resourceFile->getPath().generic_string());
}

void ResourceLoader::ReloadResourceData(const String& path)
{
	FileBuffer& buffer = OS::LoadFileContents(path);

	for (auto&& [resData, resFile] : s_ResourcesDataFiles)
	{
		if (resData->getPath() == path)
		{
			*resData->getRawData() = buffer;
		}
	}
}

void ResourceLoader::UpdateFileTimes(ResourceFile* file)
{
	file->m_LastReadTime = OS::s_FileSystemClock.now();
	file->m_LastChangedTime = OS::GetFileLastChangedTime(file->getPath().string());
}

void ResourceLoader::Reload(TextResourceFile* file)
{
	UpdateFileTimes(file);
	ReloadResourceData(file->m_ResourceData->getPath().string());
}

void ResourceLoader::Reload(LuaTextResourceFile* file)
{
	Reload((TextResourceFile*)file);
}

void ResourceLoader::Reload(AudioResourceFile* file)
{
	UpdateFileTimes(file);
	ReloadResourceData(file->getPath().string());
	const char* audioBuffer;
	int format;
	int size;
	float frequency;
	ALUT_CHECK(audioBuffer = (const char*)alutLoadMemoryFromFile(
	               OS::GetAbsolutePath(file->m_ResourceData->getPath().string()).string().c_str(),
	               &format,
	               &size,
	               &frequency));
	file->m_ResourceData->getRawData()->clear();
	file->m_ResourceData->getRawData()->insert(
	    file->m_ResourceData->getRawData()->begin(),
	    audioBuffer,
	    audioBuffer + size);

	AudioResourceFile* audioRes = file;
	LoadALUT(audioRes, audioBuffer, format, size, frequency);
}

void ResourceLoader::Reload(ModelResourceFile* file)
{
	UpdateFileTimes(file);
	ReloadResourceData(file->getPath().string());
	LoadAssimp(file);
}

void ResourceLoader::Reload(ImageResourceFile* file)
{
	UpdateFileTimes(file);
	ReloadResourceData(file->m_ResourceData->getPath().string());
}

void ResourceLoader::Reload(FontResourceFile* file)
{
	UpdateFileTimes(file);
	ReloadResourceData(file->m_ResourceData->getPath().string());
	file->regenerateFont();
}

int ResourceLoader::Preload(Vector<String> paths, Atomic<int>& progress)
{
	if (paths.empty())
	{
		PRINT("Asked to preload an empty list of files, no files preloaded");
		return 0;
	}

	std::sort(paths.begin(), paths.end());
	Vector<String> empericalPaths = { paths.front() };
	for (auto& incomingPath : paths)
	{
		if (empericalPaths.back() != incomingPath)
		{
			empericalPaths.emplace_back(incomingPath);
		}
	}

	ThreadPool& preloadThreads = Application::GetSingleton()->getThreadPool();
	Vector<Ref<Task>> preloadTasks;

	progress = 0;
	for (auto& path : empericalPaths)
	{
		Ref<Task> loadingTask(new Task([=, &progress]() {
			CreateSomeResourceFile(path);
			progress++;
		}));
		preloadTasks.push_back(loadingTask);
	}

	preloadTasks.push_back(Ref<Task>(new Task([]() {})));

	preloadThreads.submit(preloadTasks);

	PRINT("Preloading " + std::to_string(paths.size()) + " resource files");
	return preloadTasks.size() - 1; // 1 less for the dummy task
}

void ResourceLoader::Unload(const Vector<String>& paths)
{
	Vector<const Ptr<ResourceData>*> unloads;
	for (auto& [data, file] : s_ResourcesDataFiles)
	{
		for (auto& path : paths)
		{
			if (data->getPath() == path)
			{
				unloads.push_back(&data);
			}
		}
	}

	for (auto& dataPtr : unloads)
	{
		s_ResourcesDataFiles.erase(*dataPtr);
	}

	PRINT("Unloaded " + std::to_string(paths.size()) + " resource files");
}
