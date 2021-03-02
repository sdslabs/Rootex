
.. _program_listing_file_rootex_framework_components_transform_animation_component.h:

Program Listing for File transform_animation_component.h
========================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_transform_animation_component.h>` (``rootex\framework\components\transform_animation_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "components/transform_component.h"
   
   class TransformAnimationComponent : public Component
   {
       DEPENDS_ON(TransformComponent);
   
   public:
       struct Keyframe
       {
           float timePosition;
           Matrix transform;
       };
   
       enum class TransitionType
       {
           SmashSmash = 0,
           EaseEase = 1,
           SmashEase = 2,
           EaseSmash = 3
       };
   
       enum class AnimationMode
       {
           None = 0,
           Looping = 1,
           Alternating = 2
       };
   
   private:
       static Component* Create(const JSON::json& componentData);
   
       friend class ECSFactory;
   
       Vector<Keyframe> m_Keyframes;
       bool m_IsPlayOnStart;
       AnimationMode m_AnimationMode;
       TransitionType m_TransitionType;
   
       bool m_IsPlaying;
       float m_CurrentTimePosition;
       float m_TimeDirection;
   
       TransformAnimationComponent(const Vector<Keyframe> keyframes, bool isPlayOnStart, AnimationMode animationMode, TransitionType transition);
       TransformAnimationComponent(TransformComponent&) = delete;
       ~TransformAnimationComponent() = default;
   
       Matrix interpolateMatrix(const Matrix& left, const Matrix& right, float lerpFactor);
   
       friend class TransformAnimationSystem;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformAnimationComponent;
   
       virtual bool setupData() override;
   
       void pushKeyframe(const Keyframe& keyFrame);
       void popKeyframe(int count);
       bool isPlaying() const { return m_IsPlaying; }
       bool isPlayOnStart() const { return m_IsPlayOnStart; }
       bool hasEnded() const;
       float getStartTime() const;
       float getEndTime() const;
       void reset();
   
       void interpolate(float t);
   
       void setPlaying(bool enabled);
   
       virtual void onTrigger() override;
       virtual const char* getName() const override { return "TransformAnimationComponent"; }
       ComponentID getComponentID() const { return s_ID; }
       virtual JSON::json getJSON() const override;
   
       virtual void draw() override;
   };
