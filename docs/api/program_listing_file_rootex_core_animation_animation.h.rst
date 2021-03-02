
.. _program_listing_file_rootex_core_animation_animation.h:

Program Listing for File animation.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_animation_animation.h>` (``rootex\core\animation\animation.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   struct TranslationKeyframe
   {
       float m_Time;
       Vector3 m_Translation;
   };
   
   struct RotationKeyframe
   {
       float m_Time;
       Quaternion m_Rotation;
   };
   
   struct ScalingKeyframe
   {
       float m_Time;
       Vector3 m_Scaling;
   };
   
   struct SkeletonNode
   {
       Vector<Ptr<SkeletonNode>> m_Children;
       String m_Name;
       Matrix m_LocalBindTransform;
   };
   
   class BoneAnimation
   {
       Vector<TranslationKeyframe> m_Translation;
       Vector<RotationKeyframe> m_Rotation;
       Vector<ScalingKeyframe> m_Scaling;
   
       float getEndTime() const;
       float getStartTime() const;
   
   public:
       BoneAnimation() = default;
       BoneAnimation(const BoneAnimation&) = default;
       ~BoneAnimation() = default;
   
       void addTranslationKeyframe(TranslationKeyframe& keyframe) { m_Translation.push_back(keyframe); }
       void addRotationKeyframe(RotationKeyframe& keyframe) { m_Rotation.push_back(keyframe); }
       void addScalingKeyframe(ScalingKeyframe& keyframe) { m_Scaling.push_back(keyframe); }
   
       Matrix interpolate(float time);
   };
   
   class SkeletalAnimation
   {
       float m_Duration;
       HashMap<String, BoneAnimation> m_BoneAnimations;
   
   public:
       SkeletalAnimation() = default;
       SkeletalAnimation(const SkeletalAnimation&) = default;
       ~SkeletalAnimation() = default;
   
       Matrix interpolate(const String& nodeName, float currentTime);
   
       float getEndTime() const;
   
       void setDuration(float time) { m_Duration = time; }
       void addBoneAnimation(String boneName, BoneAnimation& boneAnimation) { m_BoneAnimations[boneName] = boneAnimation; }
   };
