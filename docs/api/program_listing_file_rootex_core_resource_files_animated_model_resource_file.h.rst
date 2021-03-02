
.. _program_listing_file_rootex_core_resource_files_animated_model_resource_file.h:

Program Listing for File animated_model_resource_file.h
=======================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_animated_model_resource_file.h>` (``rootex\core\resource_files\animated_model_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   #include "renderer/mesh.h"
   #include <Assimp/scene.h>
   #include "core/animation/animation.h"
   
   class Material;
   
   class AnimatedModelResourceFile : public ResourceFile
   {
       explicit AnimatedModelResourceFile(const FilePath& resData);
   
       Vector<Pair<Ref<Material>, Vector<Mesh>>> m_Meshes;
   
       HashMap<String, unsigned int> m_BoneMapping;
       Vector<Matrix> m_BoneOffsets;
       Vector<Matrix> m_AnimationTransforms;
       Matrix m_RootInverseTransform;
   
       Ptr<SkeletonNode> m_RootNode;
       HashMap<String, SkeletalAnimation> m_Animations;
   
       friend class ResourceLoader;
   
   public:
       static Matrix AiMatrixToMatrix(const aiMatrix4x4& aiMatrix);
   
       explicit AnimatedModelResourceFile(AnimatedModelResourceFile&) = delete;
       explicit AnimatedModelResourceFile(AnimatedModelResourceFile&&) = delete;
       ~AnimatedModelResourceFile() = default;
   
       void reimport() override;
   
       Vector<Pair<Ref<Material>, Vector<Mesh>>>& getMeshes() { return m_Meshes; }
       HashMap<String, SkeletalAnimation>& getAnimations() { return m_Animations; }
       size_t getBoneCount() const { return m_BoneOffsets.size(); }
   
       void setNodeHierarchy(aiNode* currentAiNode, Ptr<SkeletonNode>& currentNode);
       void setAnimationTransforms(Ptr<SkeletonNode>& node, float currentTime, const String& animationName, const Matrix& parentModelTransform, bool isRootFound);
   
       Vector<String> getAnimationNames();
       float getAnimationEndTime(const String& animationName);
       void getFinalTransforms(const String& animationName, float currentTime, Vector<Matrix>& transforms);
   };
