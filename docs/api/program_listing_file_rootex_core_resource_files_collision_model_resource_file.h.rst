
.. _program_listing_file_rootex_core_resource_files_collision_model_resource_file.h:

Program Listing for File collision_model_resource_file.h
========================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_collision_model_resource_file.h>` (``rootex\core\resource_files\collision_model_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "renderer/mesh.h"
   #include "resource_file.h"
   
   #include "btBulletCollisionCommon.h"
   
   class CollisionModelResourceFile : public ResourceFile
   {
       explicit CollisionModelResourceFile(const FilePath& path);
   
       btTriangleIndexVertexArray m_TriangleMesh;
       Vector<btIndexedMesh> m_Meshes;
       Vector<Vector3> m_Vertices;
       Vector<short> m_Indices;
   
       friend class ResourceLoader;
   
   public:
       explicit CollisionModelResourceFile(const CollisionModelResourceFile&) = delete;
       explicit CollisionModelResourceFile(const CollisionModelResourceFile&&) = delete;
       ~CollisionModelResourceFile() = default;
   
       void reimport() override;
   
       btTriangleIndexVertexArray* getCollisionMesh() { return &m_TriangleMesh; }
   };
