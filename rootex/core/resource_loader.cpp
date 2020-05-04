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

bool IsSupported(const String& extension, const Vector<String>& supportedExtensions)
{
	return std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end();
}

void ResourceLoader::UpdateFileTimes(ResourceFile* resourceFile)
{
	resourceFile->m_LastReadTime = OS::s_FileSystemClock.now();
	resourceFile->m_LastChangedTime = OS::GetFileLastChangedTime(resourceFile->getPath().string());
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

void FillBoneHierarchy(Vector<int>* boneHierarchy, Map<String, int>* boneNameID, aiNode* currentNode)
{
	int newID = boneHierarchy->back() + 1;
	boneHierarchy->push_back(newID);
	boneNameID->insert({ currentNode->mName.C_Str(), newID });

	for (int i = 0; i < currentNode->mNumChildren; i++)
	{
		FillBoneHierarchy(boneHierarchy, boneNameID, currentNode->mChildren[i]);
	}
}

SkeletalAnimationResourceFile* ResourceLoader::CreateSkeletalAnimationResourceFile(const String& path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::Animation)
		{
			return reinterpret_cast<SkeletalAnimationResourceFile*>(item.second.get());
		}
	}

	if (OS::IsExists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path,
	    aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	PANIC(scene == nullptr, "Model could not be loaded: " + OS::GetAbsolutePath(path).generic_string());

	if (!scene->HasAnimations())
	{
		return nullptr;
	}

	Vector<VertexData> vertices;
	const aiMesh* mesh = scene->mMeshes[0]; // Taking single mesh
	vertices.reserve(mesh->mNumVertices);

	VertexData vertex;
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
	aiFace* face;
	for (unsigned int f = 0; f < mesh->mNumFaces; f++)
	{
		face = &mesh->mFaces[f];
		//Model already triangulated by aiProcess_Triangulate so no need to check
		indices.push_back(face->mIndices[0]);
		indices.push_back(face->mIndices[1]);
		indices.push_back(face->mIndices[2]);
	}

	Vector<int> boneHierarchy;
	boneHierarchy.push_back(0);
	Map<String, int> boneNameID;
	boneNameID[scene->mRootNode->mName.C_Str()] = 0;
	FillBoneHierarchy(&boneHierarchy, &boneNameID, scene->mRootNode);

	Vector<Matrix> boneOffsets;
	boneOffsets.resize(boneHierarchy.size());
	
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		const String& boneName = mesh->mBones[boneIndex]->mName.C_Str();

		aiMatrix4x4& offset = mesh->mBones[boneIndex]->mOffsetMatrix;
		Matrix offsetMatrix = Matrix({ 
			offset.a1,
		    offset.a2,
		    offset.a3,
		    offset.a4,

		    offset.b1,
		    offset.b2,
		    offset.b3,
		    offset.b4,
		    
			offset.c1,
		    offset.c2,
		    offset.c3,
		    offset.c4,
		    
			offset.d1,
		    offset.d2,
		    offset.d3,
		    offset.d4 });

		boneOffsets[boneNameID[boneName]] = offsetMatrix;
	}

	Map<String, SkeletalAnimation> animations;
	
	for (int animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
	{
		aiAnimation* currentAnimation = scene->mAnimations[animationIndex];

		SkeletalAnimation animation;
		for (auto& bone : boneHierarchy)
		{
			BasicAnimation boneAnimation;

			boneAnimation.m_TranslationKeyframes.resize(currentAnimation->mNumChannels);
			boneAnimation.m_RotationKeyframes.resize(currentAnimation->mNumChannels);
			boneAnimation.m_ScaleKeyframes.resize(currentAnimation->mNumChannels);

			for (int channelIndex = 0; channelIndex < currentAnimation->mNumChannels; channelIndex++)
			{
				aiNodeAnim* currentChannel = currentAnimation->mChannels[channelIndex];
				int animationBoneID = boneNameID[currentChannel->mNodeName.C_Str()];
				
				for (int p = 0; p < currentChannel->mNumPositionKeys; p++)
				{
					boneAnimation.m_TranslationKeyframes[p] = { 
						currentChannel->mPositionKeys[p].mTime, 
						{ 
							currentChannel->mPositionKeys[p].mValue.x, 
							currentChannel->mPositionKeys[p].mValue.y, 
							currentChannel->mPositionKeys[p].mValue.z 
						} 
					};
				}
				for (int p = 0; p < currentChannel->mNumRotationKeys; p++)
				{
					boneAnimation.m_RotationKeyframes[p] = { 
						currentChannel->mRotationKeys[p].mTime,
					    { 
							currentChannel->mRotationKeys[p].mValue.x,
					        currentChannel->mRotationKeys[p].mValue.y,
					        currentChannel->mRotationKeys[p].mValue.z,
					        currentChannel->mRotationKeys[p].mValue.w 
						} 
					};
				}
				for (int p = 0; p < currentChannel->mNumScalingKeys; p++)
				{
					boneAnimation.m_ScaleKeyframes[p] = { 
						currentChannel->mScalingKeys[p].mTime,
					    { 
							currentChannel->mScalingKeys[p].mValue.x,
					        currentChannel->mScalingKeys[p].mValue.y,
					        currentChannel->mScalingKeys[p].mValue.z 
						} 
					};
				}
			}

			animation.m_BoneAnimations.push_back(boneAnimation);
		}

		animations[currentAnimation->mName.C_Str()] = animation;
	}

	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	Ptr<VertexBuffer> vertexBuffer(new VertexBuffer(vertices));
	Ptr<IndexBuffer> indexBuffer(new IndexBuffer(indices));
	SkeletalAnimationResourceFile* animRes = new SkeletalAnimationResourceFile(boneHierarchy, boneOffsets, animations, std::move(vertexBuffer), std::move(indexBuffer), resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(animRes);
	s_ResourceFileLibrary[ResourceFile::Type::Animation].push_back(animRes);

	return animRes;
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

	Assimp::Importer importer;

	// File not found in cache, load it only once
	const aiScene* scene = importer.ReadFile(path,
	    aiProcess_CalcTangentSpace | 
		aiProcess_Triangulate | 
		aiProcess_JoinIdenticalVertices | 
		aiProcess_SortByPType);
    const aiMesh* mesh = scene->mMeshes[0]; //Taking single mesh
	aiFace* face;

	PANIC(scene == nullptr, "Model could not be loaded: " + OS::GetAbsolutePath(path).generic_string());
	
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

void ResourceLoader::Reload(TextResourceFile* textFile)
{
	UpdateFileTimes(textFile);
	ResourceLoader::ReloadResourceData(textFile->m_ResourceData->getPath().string());
}

void ResourceLoader::Reload(AudioResourceFile* audioRes)
{
	UpdateFileTimes(audioRes);
	const char* audioBuffer;
	int format;
	int size;
	float frequency;
	ALUT_CHECK(audioBuffer = (const char*)alutLoadMemoryFromFile(
	               OS::GetAbsolutePath(audioRes->m_ResourceData->getPath().string()).string().c_str(),
	               &format,
	               &size,
	               &frequency));

	audioRes->m_ResourceData->getRawData()->clear();
	audioRes->m_ResourceData->getRawData()->insert(
	    audioRes->m_ResourceData->getRawData()->begin(),
	    audioBuffer,
	    audioBuffer + size);

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

void ResourceLoader::Reload(SkeletalAnimationResourceFile* animFile)
{
	UpdateFileTimes(animFile);
}

void ResourceLoader::Reload(VisualModelResourceFile* modelFile)
{
	UpdateFileTimes(modelFile);

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile((OS::GetAbsolutePath(modelFile->m_ResourceData->getPath().string()).generic_string()),
	    aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	PANIC(scene == nullptr, "Model could not be loaded: " + OS::GetAbsolutePath(modelFile->m_ResourceData->getPath().string()).generic_string());

	const aiMesh* mesh = scene->mMeshes[0];
	aiFace* face;

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

	ResourceLoader::ReloadResourceData(modelFile->m_ResourceData->getPath().string());
	modelFile->m_VertexBuffer.reset(new VertexBuffer(vertices));
	modelFile->m_IndexBuffer.reset(new IndexBuffer(indices));
}

void ResourceLoader::Reload(ImageResourceFile* imageFile)
{
	UpdateFileTimes(imageFile);
	ResourceLoader::ReloadResourceData(imageFile->m_ResourceData->getPath().string());
}

void ResourceLoader::Reload(FontResourceFile* fontFile)
{
	UpdateFileTimes(fontFile);
	ResourceLoader::ReloadResourceData(fontFile->m_ResourceData->getPath().string());
}

Vector<ResourceFile*>& ResourceLoader::GetFilesOfType(ResourceFile::Type type)
{
	return s_ResourceFileLibrary[type];
}

HashMap<ResourceFile::Type, Vector<ResourceFile*>>& ResourceLoader::GetAllFiles()
{
	return s_ResourceFileLibrary;
}
