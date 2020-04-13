#include "resource_loader.h"

#include <d3d11.h>

#include "common/common.h"

#include "framework/systems/audio_system.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/vertex_data.h"
#include "script/interpreter.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> ResourceLoader::s_ResourcesDataFiles;
HashMap<ResourceFile::Type, Vector<ResourceFile*>> ResourceLoader::s_ResourceFileLibrary;
Assimp::Importer ResourceLoader::s_ModelLoader;

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
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Wav)
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

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(audioRes);
	s_ResourceFileLibrary[ResourceFile::Type::Wav].push_back(audioRes);

	return audioRes;
}

VisualModelResourceFile* ResourceLoader::CreateVisualModelResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Obj)
		{
			return reinterpret_cast<VisualModelResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	const aiScene* scene = s_ModelLoader.ReadFile(path,
	      aiProcess_CalcTangentSpace      |
		  aiProcess_Triangulate           | 
		  aiProcess_JoinIdenticalVertices | 
		  aiProcess_SortByPType);
    const aiMesh* mesh = scene->mMeshes[0]; //Taking single mesh
	aiFace* face;

	// File not found in cache, load it only once
	PANIC(scene == false, "Model could not be loaded: " + OS::GetAbsolutePath(path).generic_string());

	VertexData vertex;

	Vector<VertexData> vertices;
	vertices.reserve(mesh->mNumVertices);

	for (unsigned int v = 0; v < mesh->mNumVertices; v++)
	{
		vertex.m_Position.x = mesh->mVertices[v].x;
		vertex.m_Position.y = mesh->mVertices[v].y;
		vertex.m_Position.z = mesh->mVertices[v].z;
		vertex.m_Normal.x = mesh->mNormals[v].x;
		vertex.m_Normal.y = mesh->mNormals[v].y;
		vertex.m_Normal.z = mesh->mNormals[v].z;
		// Assuming the model has texture coordinates and taking only the first texture coordinate in case of multiple texture coordinates
		vertex.m_TextureCoord.x = mesh->mTextureCoords[0][v].x;
		vertex.m_TextureCoord.y = mesh->mTextureCoords[0][v].y;
		vertices.push_back(vertex);
	}

	std::vector<unsigned short> indices;

	for (unsigned int f = 0; f < mesh->mNumFaces; f++)
	{
		face = &mesh->mFaces[f];
		//Model already triangulated by aiProcess_Triangulate so no need to check
		indices.push_back(face->mIndices[0]);
		indices.push_back(face->mIndices[1]);
		indices.push_back(face->mIndices[2]);
	}

	FileBuffer& buffer = OS::LoadFileContents(path);
	Ptr<VertexBuffer> vertexBuffer(new VertexBuffer(vertices));
	Ptr<IndexBuffer> indexBuffer(new IndexBuffer(indices));

	ResourceData* resData = new ResourceData(path, buffer);
	VisualModelResourceFile* visualRes = new VisualModelResourceFile(std::move(vertexBuffer), std::move(indexBuffer), resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(visualRes);
	s_ResourceFileLibrary[ResourceFile::Type::Obj].push_back(visualRes);

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

Vector<ResourceFile*>& ResourceLoader::GetFilesOfType(ResourceFile::Type type)
{
	return s_ResourceFileLibrary[type];
}

HashMap<ResourceFile::Type, Vector<ResourceFile*>>& ResourceLoader::GetAllFiles()
{
	return s_ResourceFileLibrary;
}
