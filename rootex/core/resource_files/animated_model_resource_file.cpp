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

Matrix AnimatedModelResourceFile::AiMatrixToMatrix(const aiMatrix4x4& aiMatrix)
{
	return Matrix(
	    aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
	    aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
	    aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
	    aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4)
	    .Transpose();
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

void AnimatedModelResourceFile::setNodeHierarchy(aiNode* currentAiNode, Ptr<SkeletonNode>& currentNode)
{
	currentNode->m_Name = String(currentAiNode->mName.C_Str());
	currentNode->m_LocalBindTransform = AiMatrixToMatrix(currentAiNode->mTransformation);

	currentNode->m_Children.resize(currentAiNode->mNumChildren);
	for (int i = 0; i < currentAiNode->mNumChildren; i++)
	{
		currentNode->m_Children[i] = std::make_unique<SkeletonNode>(SkeletonNode());
		setNodeHierarchy(currentAiNode->mChildren[i], currentNode->m_Children[i]);
	}
}

void AnimatedModelResourceFile::getFinalTransforms(const String& animationName, float currentTime, Vector<Matrix>& transforms)
{
	bool rootFoundFlag = false;
	setAnimationTransforms(m_RootNode, currentTime, animationName, Matrix::Identity, rootFoundFlag);

	for (unsigned int i = 0; i < getBoneCount(); i++)
	{
		transforms[i] = m_BoneOffsets[i] * m_AnimationTransforms[i] * m_RootInverseTransform;
	}
}

void AnimatedModelResourceFile::setAnimationTransforms(Ptr<SkeletonNode>& node, float currentTime, const String& animationName, const Matrix& parentModelTransform, bool isRootFound)
{
	Matrix boneSpaceTransform = m_Animations[animationName].interpolate(node->m_Name.c_str(), currentTime);
	if (boneSpaceTransform == Matrix::Identity)
	{
		boneSpaceTransform = node->m_LocalBindTransform;
	}

	Matrix currentModelTransform = boneSpaceTransform * parentModelTransform;

	if (m_BoneMapping.find(node->m_Name.c_str()) != m_BoneMapping.end())
	{
		if (!isRootFound)
		{
			m_RootInverseTransform = currentModelTransform.Invert();
			isRootFound = true;
		}

		m_AnimationTransforms[m_BoneMapping[node->m_Name]] = currentModelTransform;
	}

	for (auto& child : node->m_Children)
	{
		setAnimationTransforms(child, currentTime, animationName, currentModelTransform, isRootFound);
	}
}

void AnimatedModelResourceFile::reimport()
{
	ResourceFile::reimport();

	Assimp::Importer animatedModelLoader;
	const aiScene* scene = animatedModelLoader.ReadFile(
	    getPath().generic_string(),
	    aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SplitLargeMeshes | aiProcess_GenBoundingBoxes | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure | aiProcess_ConvertToLeftHanded);

	if (!scene)
	{
		ERR("Model could not be loaded: " + getPath().generic_string());
		ERR("Assimp: " + animatedModelLoader.GetErrorString());
		return;
	}
	if (scene->mNumAnimations == 0)
	{
		ERR("No animations found in file: " + getPath().generic_string());
		return;
	}

	unsigned int boneCount = 0;
	m_Meshes.clear();

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

			if (mesh->mTangents)
			{
				vertex.m_Tangent.x = mesh->mTangents[j].x;
				vertex.m_Tangent.y = mesh->mTangents[j].y;
				vertex.m_Tangent.z = mesh->mTangents[j].z;
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
		float alpha = 1.0f;
		if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
			WARN("Material does not have color: " + String(material->GetName().C_Str()));
		}
		if (AI_SUCCESS != material->Get(AI_MATKEY_OPACITY, alpha))
		{
			WARN("Material does not have alpha: " + String(material->GetName().C_Str()));
		}

		Ref<AnimatedMaterial> extractedMaterial;

		String materialPath;

		if (String(material->GetName().C_Str()) == "DefaultMaterial")
		{
			materialPath = MaterialLibrary::s_AnimatedDefaultMaterialPath;
		}
		else
		{
			materialPath = "game/assets/materials/" + String(material->GetName().C_Str()) + ".rmat";
		}

		if (OS::IsExists(materialPath))
		{
			extractedMaterial = std::dynamic_pointer_cast<AnimatedMaterial>(MaterialLibrary::GetMaterial(materialPath));
			if (!extractedMaterial)
			{
				WARN("Material loaded was not an AnimatedMaterial. Replacing with default AnimatedMaterial: " + materialPath);
				extractedMaterial = std::dynamic_pointer_cast<AnimatedMaterial>(MaterialLibrary::GetDefaultAnimatedMaterial());
			}
		}
		else
		{
			MaterialLibrary::CreateNewMaterialFile(materialPath, "AnimatedMaterial");
			extractedMaterial = std::dynamic_pointer_cast<AnimatedMaterial>(MaterialLibrary::GetMaterial(materialPath));
			extractedMaterial->setColor({ color.r, color.g, color.b, alpha });

			for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
			{
				aiString diffuseStr;
				material->GetTexture(aiTextureType_DIFFUSE, i, &diffuseStr);

				bool isEmbedded = *diffuseStr.C_Str() == '*';
				if (!isEmbedded)
				{
					// Texture is given as a path
					String texturePath = diffuseStr.C_Str();
					ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(getPath().parent_path().generic_string() + "/" + texturePath);

					if (image)
					{
						extractedMaterial->setDiffuseTexture(image);
					}
					else
					{
						WARN("Could not set material diffuse texture: " + texturePath);
					}
				}
				else
				{
					WARN("Embedded diffuse texture found in material: " + extractedMaterial->getFullName() + ". Embedded textures are unsupported.");
				}
			}

			for (int i = 0; i < material->GetTextureCount(aiTextureType_NORMALS); i++)
			{
				aiString normalStr;
				material->GetTexture(aiTextureType_NORMALS, i, &normalStr);
				bool isEmbedded = *normalStr.C_Str() == '*';
				if (isEmbedded)
				{
					String texturePath = normalStr.C_Str();
					ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(getPath().parent_path().generic_string() + "/" + texturePath);

					if (image)
					{
						extractedMaterial->setNormalTexture(image);
					}
					else
					{
						WARN("Could not set material normal map texture: " + texturePath);
					}
				}
				else
				{
					WARN("Embedded normal texture found in material: " + extractedMaterial->getFullName() + ". Embedded textures are unsupported.");
				}
			}

			for (int i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
			{
				aiString specularStr;
				material->GetTexture(aiTextureType_SPECULAR, i, &specularStr);
				bool isEmbedded = *specularStr.C_Str() == '*';
				if (isEmbedded)
				{
					String texturePath = specularStr.C_Str();
					ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(getPath().parent_path().generic_string() + "/" + texturePath);

					if (image)
					{
						extractedMaterial->setSpecularTexture(image);
					}
					else
					{
						WARN("Could not set material specular map texture: " + texturePath);
					}
				}
				else
				{
					WARN("Embedded specular texture found in material: " + extractedMaterial->getFullName() + ". Embedded textures are unsupported.");
				}
			}
		}

		HashMap<int, Vector<unsigned int>> verticesIndex;
		HashMap<int, Vector<float>> verticesWeights;

		for (int j = 0; j < mesh->mNumBones; j++)
		{
			unsigned int boneIndex = 0;
			const aiBone* bone = mesh->mBones[j];

			if (m_BoneMapping.find(bone->mName.C_Str()) == m_BoneMapping.end())
			{
				boneIndex = boneCount;
				boneCount++;

				m_BoneMapping[bone->mName.C_Str()] = boneIndex;

				Matrix offsetMatrix = AiMatrixToMatrix(bone->mOffsetMatrix);
				m_BoneOffsets.push_back(offsetMatrix);
			}
			else
			{
				boneIndex = m_BoneMapping[bone->mName.C_Str()];
			}

			for (int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++)
			{
				verticesIndex[bone->mWeights[weightIndex].mVertexId].push_back(boneIndex);
				verticesWeights[bone->mWeights[weightIndex].mVertexId].push_back(bone->mWeights[weightIndex].mWeight);
			}
		}

		for (auto& [vertexID, boneIndices] : verticesIndex)
		{
			boneIndices.resize(4);
			vertices[vertexID].m_BoneIndices[0] = boneIndices[0];
			vertices[vertexID].m_BoneIndices[1] = boneIndices[1];
			vertices[vertexID].m_BoneIndices[2] = boneIndices[2];
			vertices[vertexID].m_BoneIndices[3] = boneIndices[3];
		}

		for (auto& [vertexID, boneWeights] : verticesWeights)
		{
			boneWeights.resize(4);
			vertices[vertexID].m_BoneWeights.x = boneWeights[0];
			vertices[vertexID].m_BoneWeights.y = boneWeights[1];
			vertices[vertexID].m_BoneWeights.z = boneWeights[2];
			vertices[vertexID].m_BoneWeights.w = boneWeights[3];
		}

		Mesh extractedMesh;
		extractedMesh.m_VertexBuffer.reset(new VertexBuffer(vertices));
		extractedMesh.m_IndexBuffer.reset(new IndexBuffer(indices));
		Vector3 max = { mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z };
		Vector3 min = { mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z };

		Matrix rotationFix = Matrix(DirectX::XMMatrixRotationNormal({ 1, 0, 0 }, 1.5708));
		max = DirectX::XMVector3Transform(max, rotationFix);
		min = DirectX::XMVector3Transform(min, rotationFix);

		Vector3 center = (max + min) / 2.0f;
		extractedMesh.m_BoundingBox.Center = center;
		extractedMesh.m_BoundingBox.Extents = (max - min) / 2.0f;

		bool found = false;
		for (auto& materialModels : getMeshes())
		{
			if (materialModels.first == extractedMaterial)
			{
				found = true;
				materialModels.second.push_back(extractedMesh);
				break;
			}
		}

		if (!found && extractedMaterial)
		{
			getMeshes().push_back(Pair<Ref<Material>, Vector<Mesh>>(extractedMaterial, { extractedMesh }));
		}
	}

	m_RootNode = std::make_unique<SkeletonNode>(SkeletonNode());
	setNodeHierarchy(scene->mRootNode, m_RootNode);

	m_AnimationTransforms.resize(boneCount);

	for (int i = 0; i < scene->mNumAnimations; i++)
	{
		const aiAnimation* anim = scene->mAnimations[i];

		SkeletalAnimation animation;

		float durationInTicks = anim->mDuration;
		float ticksPerSecond = anim->mTicksPerSecond ? anim->mTicksPerSecond : 25.0f;
		float durationInSeconds = durationInTicks / ticksPerSecond;

		animation.setDuration(durationInSeconds);

		for (int j = 0; j < anim->mNumChannels; j++)
		{
			const aiNodeAnim* nodeAnim = anim->mChannels[j];

			BoneAnimation boneAnims;
			for (int k = 0; k < nodeAnim->mNumPositionKeys; k++)
			{
				TranslationKeyframe keyframe;

				keyframe.m_Time = nodeAnim->mPositionKeys[k].mTime / ticksPerSecond;

				keyframe.m_Translation.x = nodeAnim->mPositionKeys[k].mValue.x;
				keyframe.m_Translation.y = nodeAnim->mPositionKeys[k].mValue.y;
				keyframe.m_Translation.z = nodeAnim->mPositionKeys[k].mValue.z;

				boneAnims.addTranslationKeyframe(keyframe);
			}

			for (int k = 0; k < nodeAnim->mNumRotationKeys; k++)
			{
				RotationKeyframe keyframe;

				keyframe.m_Time = nodeAnim->mRotationKeys[k].mTime / ticksPerSecond;

				keyframe.m_Rotation.x = nodeAnim->mRotationKeys[k].mValue.x;
				keyframe.m_Rotation.y = nodeAnim->mRotationKeys[k].mValue.y;
				keyframe.m_Rotation.z = nodeAnim->mRotationKeys[k].mValue.z;
				keyframe.m_Rotation.w = nodeAnim->mRotationKeys[k].mValue.w;

				boneAnims.addRotationKeyframe(keyframe);
			}

			for (int k = 0; k < nodeAnim->mNumScalingKeys; k++)
			{
				ScalingKeyframe keyframe;

				keyframe.m_Time = nodeAnim->mScalingKeys[k].mTime / ticksPerSecond;

				keyframe.m_Scaling.x = nodeAnim->mScalingKeys[k].mValue.x;
				keyframe.m_Scaling.y = nodeAnim->mScalingKeys[k].mValue.y;
				keyframe.m_Scaling.z = nodeAnim->mScalingKeys[k].mValue.z;

				boneAnims.addScalingKeyframe(keyframe);
			}
			animation.addBoneAnimation(nodeAnim->mNodeName.C_Str(), boneAnims);
		}
		m_Animations[anim->mName.C_Str()] = animation;
	}
}
