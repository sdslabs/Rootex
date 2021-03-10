#include "collision_model_resource_file.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

CollisionModelResourceFile::CollisionModelResourceFile(const FilePath& path)
    : ResourceFile(Type::CollisionModel, path)
{
	reimport();
}

void CollisionModelResourceFile::reimport()
{
	ResourceFile::reimport();

	Assimp::Importer modelLoader;
	const aiScene* scene = modelLoader.ReadFile(
	    getPath().generic_string(),
	    aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_RemoveComponent);

	if (!scene)
	{
		ERR("Collision Model could not be loaded: " + getPath().generic_string());
		ERR("Assimp: " + modelLoader.GetErrorString());
		return;
	}

	btTriangleIndexVertexArray meshes;
	m_Meshes.clear();
	m_Vertices.clear();
	m_Indices.clear();

	// Stop the arrays from changing memory locations
	unsigned int totalVertices = 0;
	unsigned int totalIndices = 0;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		totalVertices += scene->mMeshes[i]->mNumVertices;
		totalIndices += scene->mMeshes[i]->mNumFaces * 3;
	}
	m_Vertices.reserve(totalVertices);
	m_Indices.reserve(totalIndices);

	unsigned int indicesSeen = 0;
	unsigned int verticesSeen = 0;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];

		btIndexedMesh indexedMesh;
		{
			indexedMesh.m_numTriangles = mesh->mNumFaces;
			indexedMesh.m_numVertices = mesh->mNumVertices;
			aiFace* face = nullptr;
			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				face = &mesh->mFaces[f];
				//Model already triangulated by aiProcess_Triangulate so no need to check
				m_Indices.push_back(face->mIndices[0]);
				m_Indices.push_back(face->mIndices[1]);
				m_Indices.push_back(face->mIndices[2]);
			}
			indexedMesh.m_triangleIndexBase = (const unsigned char*)(m_Indices.data() + indicesSeen);
			indexedMesh.m_triangleIndexStride = 3 * sizeof(short);
			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				Vector3 vertex;
				vertex.x = mesh->mVertices[v].x;
				vertex.y = mesh->mVertices[v].y;
				vertex.z = mesh->mVertices[v].z;
				m_Vertices.push_back(vertex);
			}
			indexedMesh.m_vertexBase = (const unsigned char*)(m_Vertices.data() + verticesSeen);
			indexedMesh.m_vertexStride = 3 * sizeof(float);
		}
		indicesSeen += mesh->mNumFaces * 3;
		verticesSeen += mesh->mNumVertices;
		m_Meshes.push_back(indexedMesh);
		meshes.addIndexedMesh(m_Meshes.back(), PHY_SHORT);
	}

	m_TriangleMesh = meshes;
}
