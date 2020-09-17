#include "animated_model_resource_file.h"

#include "resource_loader.h"
#include "image_resource_file.h"
#include "renderer/material_library.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

AnimatedModelResourceFile::AnimatedModelResourceFile(const FilePath& path)
    : ResourceFile(Type::AnimatedModel, path)
{
	reimport();
}

void AnimatedModelResourceFile::RegisterAPI(sol::table& rootex)
{
	sol::usertype<AnimatedModelResourceFile> animatedModelResourceFile = rootex.new_usertype<AnimatedModelResourceFile>(
	    "AnimatedModelResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
}

void AnimatedModelResourceFile::setBoneTransforms(aiNode* currentNode, Matrix parentRootTransform)
{
	aiMatrix4x4 transform = currentNode->mTransformation;
	Matrix toRootTransformation = Matrix({ transform.a1, transform.a2, transform.a3, transform.a4,
	    transform.b1, transform.b2, transform.b3, transform.b4,
	    transform.c1, transform.c2, transform.c3, transform.c4 });
	Matrix currentRootTransform = (toRootTransformation) * parentRootTransform;

	UINT index = m_BoneMapping[currentNode->mName.C_Str()];
	m_BoneTransforms[index] = currentRootTransform;

	for (size_t i = 0; i < currentNode->mNumChildren; i++) {
		setBoneTransforms(currentNode->mChildren[i], currentRootTransform);
	}
}

void AnimatedModelResourceFile::getFinalTransforms(const String& animationName, float currentTime, Vector<Matrix>& transforms)
{
	m_Animations[animationName].interpolate(currentTime, transforms);

	for (UINT i = 0; i < getBoneCount(); i++)
	{
		transforms[i] *= m_BoneOffsets[i] * m_BoneTransforms[i]; // see this method of calculating
    }
}

Vector<String> AnimatedModelResourceFile::getAnimationNames()
{
	Vector<String> animationNames;
	for (auto& element : m_Animations)
	{
		animationNames.push_back(element.first);
	}
	return animationNames;
}

float AnimatedModelResourceFile::getAnimationEndTime(const String& animationName)
{
	return m_Animations[animationName].getEndTime();
}

void AnimatedModelResourceFile::reimport()
{
	ResourceFile::reimport();

	Assimp::Importer modelLoader;
    const aiScene* scene = modelLoader.ReadFile(
	    getPath().generic_string(),
	    aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_ConvertToLeftHanded);

	if (!scene)
	{
		ERR("Model could not be loaded: " + getPath().generic_string());
		ERR("Assimp: " + modelLoader.GetErrorString());
		return;
	}

	m_Meshes.clear();
	m_Meshes.reserve(scene->mNumMeshes);

    UINT boneCount = 0;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];

		Vector<AnimatedVertexData> vertices;
		vertices.reserve(mesh->mNumVertices);

		AnimatedVertexData vertex;
		ZeroMemory(&vertex, sizeof(AnimatedVertexData));

		for (int j = 0; j < mesh->mNumVertices; j++)
		{
			vertex.m_Position.x = mesh->mVertices[j].x;
			vertex.m_Position.y = mesh->mVertices[j].y;
			vertex.m_Position.z = mesh->mVertices[j].z;

			if (mesh->mNormals)
			{
				vertex.m_Normal.x = mesh->mNormals[j].x;
				vertex.m_Normal.y = mesh->mNormals[j].y;
				vertex.m_Normal.z = mesh->mNormals[j].z;
			}

			if (mesh->mTextureCoords)
			{
				if (mesh->mTextureCoords[0])
				{
					// Assuming the model has texture coordinates and taking the only the first texture coordinate in case of multiple texture coordinates
					vertex.m_TextureCoord.x = mesh->mTextureCoords[0][j].x;
					vertex.m_TextureCoord.y = mesh->mTextureCoords[0][j].y;
				}
			}

			vertices.push_back(vertex);
		}

		std::vector<unsigned short> indices;

		aiFace* face = nullptr;
		for (unsigned int f = 0; f < mesh->mNumFaces; f++)
		{
			face = &mesh->mFaces[f];
			// Model already triangulated by aiProcess_Triangulate so no need to check
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

		Ref<AnimatedMaterial> extractedMaterial;
		if (MaterialLibrary::IsExists(material->GetName().C_Str()))
		{
			extractedMaterial = std::dynamic_pointer_cast<AnimatedMaterial>(MaterialLibrary::GetMaterial(material->GetName().C_Str() + String(".rmat")));
		}
		else
		{
			MaterialLibrary::CreateNewMaterialFile(material->GetName().C_Str(), "AnimatedMaterial");
			extractedMaterial = std::dynamic_pointer_cast<AnimatedMaterial>(MaterialLibrary::GetMaterial(material->GetName().C_Str() + String(".rmat")));
			extractedMaterial->setColor({ color.r, color.g, color.b, 1.0f });

			for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE, i, &str);
				char embeddedAsterisk = *str.C_Str();

				if (embeddedAsterisk == '*')
				{
					// Texture is embedded
					int textureID = atoi(str.C_Str() + 1);

					if (m_Textures[textureID])
					{
						aiTexture* texture = scene->mTextures[textureID];
						size_t size = scene->mTextures[textureID]->mWidth;
						PANIC(texture->mHeight == 0, "Compressed texture found but expected embedded texture");
						m_Textures[textureID].reset(new Texture(reinterpret_cast<const char*>(texture->pcData), size));
					}

					extractedMaterial->setTextureInternal(m_Textures[textureID]);
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

		VertexBoneData vertexBoneData;
		ZeroMemory(&vertexBoneData, sizeof(VertexBoneData));
		for (int j = 0; j < mesh->mNumBones; j++)
		{
			for (int v = 0; v < mesh->mBones[j]->mNumWeights; v++)
			{
				vertexBoneData.m_BoneID = j;
				vertexBoneData.m_Weight = mesh->mBones[j]->mWeights[v].mWeight;
				vertices[mesh->mBones[j]->mWeights[v].mVertexId].m_VertexBoneData.push_back(vertexBoneData);
			}
		}

        HashMap<int, Vector<UINT>> verticesIndex;
        HashMap<int, Vector<float>> verticesWeights;
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
        {
            const aiBone* bone = mesh->mBones[boneIndex];

			m_BoneMapping[bone->mName.C_Str()] = boneCount + boneIndex;

			const aiMatrix4x4& offset = bone->mOffsetMatrix;
			Matrix offsetMatrix = Matrix({ offset.a1, offset.a2, offset.a3, offset.a4,
			    offset.b1, offset.b2, offset.b3, offset.b4,
			    offset.c1, offset.c2, offset.c3, offset.c4 });
			m_BoneOffsets.push_back(offsetMatrix);

			for (int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++)
            {
                verticesIndex[bone->mWeights[weightIndex].mVertexId].push_back(boneCount + boneIndex);
                verticesWeights[bone->mWeights[weightIndex].mVertexId].push_back(bone->mWeights[weightIndex].mWeight);
            }
        }

    	AnimatedMesh extractedMesh;
		extractedMesh.m_NumBones = mesh->mNumBones;
		extractedMesh.m_VertexBuffer.reset(new VertexBuffer(vertices));
		extractedMesh.m_IndexBuffer.reset(new IndexBuffer(indices));

		m_Meshes[extractedMaterial].push_back(extractedMesh);
    }

    for (int i = 0; i < scene->mNumAnimations; i++)
    {
        const aiAnimation* anim = scene->mAnimations[i];
        String AnimationName = anim->mName.C_Str();

        SkeletalAnimation animation;

        float durationInTicks = anim->mDuration;
        float TicksPerSecond = anim->mTicksPerSecond;
        float durationInSeconds = durationInSeconds / TicksPerSecond;

        animation.m_Duration = durationInSeconds;

        animation.m_BoneAnimations.resize(anim->mNumChannels);

        for (int j = 0; j < anim->mNumChannels; j++)
        {
            const aiNodeAnim* nodeAnim = anim->mChannels[j];
            BoneAnimation boneAnims;

            for (int k = 0; k < nodeAnim->mNumPositionKeys; k++)
            {
                TranslationKeyframe keyframe;

                keyframe.m_Time = nodeAnim->mPositionKeys[k].mTime;

                keyframe.m_Translation.x = nodeAnim->mPositionKeys[k].mValue.x;
                keyframe.m_Translation.y = nodeAnim->mPositionKeys[k].mValue.y;
                keyframe.m_Translation.z = nodeAnim->mPositionKeys[k].mValue.z;

                boneAnims.m_Translation.push_back(keyframe);
            }

            for (int k = 0; k < nodeAnim->mNumRotationKeys; k++)
            {
                RotationKeyframe keyframe;

                keyframe.m_Time = nodeAnim->mRotationKeys[k].mTime;

                keyframe.m_Rotation.x = nodeAnim->mRotationKeys[k].mValue.x;
                keyframe.m_Rotation.y = nodeAnim->mRotationKeys[k].mValue.y;
                keyframe.m_Rotation.z = nodeAnim->mRotationKeys[k].mValue.z;
                keyframe.m_Rotation.w = nodeAnim->mRotationKeys[k].mValue.w;

                boneAnims.m_Rotation.push_back(keyframe);
            }

            for (int k = 0; k < nodeAnim->mNumScalingKeys; k++)
            {
                ScalingKeyframe keyframe;

                keyframe.m_Time = nodeAnim->mScalingKeys[k].mTime;

                keyframe.m_Scaling.x = nodeAnim->mScalingKeys[k].mValue.x;
                keyframe.m_Scaling.y = nodeAnim->mScalingKeys[k].mValue.y;
                keyframe.m_Scaling.z = nodeAnim->mScalingKeys[k].mValue.z;

                boneAnims.m_Scaling.push_back(keyframe);
            }
        
            int boneIndex = m_BoneMapping[nodeAnim->mNodeName.C_Str()];
            animation.m_BoneAnimations[boneIndex] = boneAnims;
        }

    m_Animations[anim->mName.C_Str()] = animation;
    }
}
