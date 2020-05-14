#include "resource_loader.h"

#include <d3d11.h>

#include "common/common.h"

#include "framework/systems/audio_system.h"
#include "core/renderer/mesh.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/material.h"
#include "core/renderer/vertex_data.h"
#include "script/interpreter.h"
#include "core/renderer/material_library.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> ResourceLoader::s_ResourcesDataFiles;
HashMap<ResourceFile::Type, Vector<ResourceFile*>> ResourceLoader::s_ResourceFileLibrary;
Assimp::Importer ResourceLoader::s_ModelLoader;

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
	const aiScene* scene = s_ModelLoader.ReadFileFromMemory(
	    file->m_ResourceData->getRawData()->data(),
	    file->m_ResourceData->getRawDataByteSize(),
	    aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (!scene)
	{
		ERR("Model could not be loaded: " + OS::GetAbsolutePath(file->m_ResourceData->getPath().string()).generic_string());
		ERR("Assimp: " + s_ModelLoader.GetErrorString());
	}

	file->m_Meshes.clear();
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
		
		if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
			WARN("Material does not have color: " + String(material->GetName().C_Str()));
		}

		MaterialLibrary::CreateNewMaterialFile(material->GetName().C_Str(), "BasicMaterial");
		Ref<BasicMaterial> extractedMaterial = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial(material->GetName().C_Str() + String(".rmat")));
		extractedMaterial->setColor({ color.r, color.g, color.b, 1.0f });
		
		Mesh extractedMesh;
		extractedMesh.m_VertexBuffer.reset(new VertexBuffer(vertices));
		extractedMesh.m_IndexBuffer.reset(new IndexBuffer(indices));
		extractedMesh.m_Material = extractedMaterial;

		file->m_Meshes.push_back(extractedMesh);
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
	s_ResourceFileLibrary[ResourceFile::Type::Text].push_back(textRes);
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
	s_ResourceFileLibrary[ResourceFile::Type::Lua].push_back(luaRes);

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
	s_ResourceFileLibrary[ResourceFile::Type::Audio].push_back(audioRes);

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
	s_ResourceFileLibrary[ResourceFile::Type::Model].push_back(visualRes);

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
	s_ResourceFileLibrary[ResourceFile::Type::Image].push_back(imageRes);

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
	s_ResourceFileLibrary[ResourceFile::Type::Font].push_back(fontRes);

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

Vector<ResourceFile*>& ResourceLoader::GetFilesOfType(ResourceFile::Type type)
{
	return s_ResourceFileLibrary[type];
}

HashMap<ResourceFile::Type, Vector<ResourceFile*>>& ResourceLoader::GetAllFiles()
{
	return s_ResourceFileLibrary;
}
