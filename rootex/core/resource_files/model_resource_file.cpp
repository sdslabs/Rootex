#include "model_resource_file.h"

#include "resource_loader.h"
#include "image_resource_file.h"
#include "renderer/material_library.h"
#include "renderer/mesh.h"
#include "renderer/vertex_buffer.h"
#include "renderer/index_buffer.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "meshoptimizer.h"

ModelResourceFile::ModelResourceFile(const FilePath& path)
    : ResourceFile(Type::Model, path)
{
	reimport();
}

void ModelResourceFile::reimport()
{
	ResourceFile::reimport();

	Assimp::Importer modelLoader;
	const aiScene* scene = modelLoader.ReadFile(
	    getPath().generic_string(),
	    aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SplitLargeMeshes | aiProcess_GenBoundingBoxes | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace | aiProcess_RemoveComponent | aiProcess_PreTransformVertices);

	if (!scene)
	{
		ERR("Model could not be loaded: " + getPath().generic_string());
		ERR("Assimp: " + modelLoader.GetErrorString());
		return;
	}

	m_Meshes.clear();
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

			if (mesh->mTangents)
			{
				vertex.m_Tangent.x = mesh->mTangents[v].x;
				vertex.m_Tangent.y = mesh->mTangents[v].y;
				vertex.m_Tangent.z = mesh->mTangents[v].z;
			}

			vertices.push_back(vertex);
		}

		Vector<unsigned int> indices;
		indices.reserve(mesh->mNumFaces);

		aiFace* face = nullptr;
		for (unsigned int f = 0; f < mesh->mNumFaces; f++)
		{
			face = &mesh->mFaces[f];
			//Model already triangulated by aiProcess_Triangulate so no need to check
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);
		}

		meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertices.size());

		Vector<Vector<unsigned int>> lods;
		float lodLevels[MAX_LOD_COUNT - 1] = { 0.8f, 0.50f, 0.3f, 0.10f };

		for (int i = 0; i < MAX_LOD_COUNT - 1; i++)
		{
			float threshold = lodLevels[i];
			size_t targetIndexCount = indices.size() * threshold;

			Vector<unsigned int> lod(indices.size());
			float lodError = 0.0f;
			size_t finalLODIndexCount = meshopt_simplifySloppy(
			    &lod[0],
			    indices.data(),
			    indices.size(),
			    &vertices[0].m_Position.x,
			    vertices.size(),
			    sizeof(VertexData),
			    targetIndexCount);
			lod.resize(finalLODIndexCount);

			lods.push_back(lod);
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

		String materialPath(material->GetName().C_Str());
		if (materialPath == "DefaultMaterial" || materialPath == "None" || materialPath.empty())
		{
			materialPath = MaterialLibrary::s_DefaultMaterialPath;
		}
		else
		{
			materialPath = "game/assets/materials/" + String(material->GetName().C_Str()) + ".rmat";
		}

		if (OS::IsExists(materialPath))
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

			for (int i = 0; i < material->GetTextureCount(aiTextureType_LIGHTMAP); i++)
			{
				aiString lightmapStr;
				material->GetTexture(aiTextureType_LIGHTMAP, i, &lightmapStr);
				bool isEmbedded = *lightmapStr.C_Str() == '*';
				if (isEmbedded)
				{
					String texturePath = lightmapStr.C_Str();
					ImageResourceFile* image = ResourceLoader::CreateImageResourceFile(getPath().parent_path().generic_string() + "/" + texturePath);

					if (image)
					{
						extractedMaterial->setLightmapTexture(image);
					}
					else
					{
						WARN("Could not set material lightmap texture: " + texturePath);
					}
				}
				else
				{
					WARN("Embedded lightmaptexture found in material: " + extractedMaterial->getFullName() + ". Embedded textures are unsupported.");
				}
			}
		}

		Mesh extractedMesh;
		extractedMesh.m_VertexBuffer.reset(new VertexBuffer(vertices));
		extractedMesh.addLOD(std::make_shared<IndexBuffer>(indices), 1.0f);
		for (int i = 0; i < MAX_LOD_COUNT - 1; i++)
		{
			if (!lods[i].empty())
			{
				extractedMesh.addLOD(std::make_shared<IndexBuffer>(lods[i]), lodLevels[i]);
			}
		}
		Vector3 max = { mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z };
		Vector3 min = { mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z };
		Vector3 center = (max + min) / 2.0f;
		extractedMesh.m_BoundingBox.Center = center;
		extractedMesh.m_BoundingBox.Extents = (max - min) / 2.0f;
		extractedMesh.m_BoundingBox.Extents.x = abs(extractedMesh.m_BoundingBox.Extents.x);
		extractedMesh.m_BoundingBox.Extents.y = abs(extractedMesh.m_BoundingBox.Extents.y);
		extractedMesh.m_BoundingBox.Extents.z = abs(extractedMesh.m_BoundingBox.Extents.z);

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
}
