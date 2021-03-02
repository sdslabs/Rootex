
.. _program_listing_file_rootex_framework_components_visual_camera_component.h:

Program Listing for File camera_component.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_camera_component.h>` (``rootex\framework\components\visual\camera_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "components/transform_component.h"
   #include "core/renderer/post_processor.h"
   
   class CameraComponent : public Component
   {
       DEPENDS_ON(TransformComponent);
   
       static Component* Create(const JSON::json& componentData);
   
       bool m_Active;
       float m_FoV;
       float m_Near;
       float m_Far;
       PostProcessingDetails m_PostProcessingDetails;
   
       Vector2 m_AspectRatio;
       Matrix m_ViewMatrix;
       Matrix m_ProjectionMatrix;
   
       CameraComponent(const Vector2& aspectRatio, float fov, float nearPlane, float farPlane, const PostProcessingDetails& postProcesing);
       CameraComponent(CameraComponent&) = delete;
       ~CameraComponent() = default;
   
       friend class ECSFactory;
   
       void refreshProjectionMatrix();
       void refreshViewMatrix();
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::CameraComponent;
   
       virtual bool setupData() override;
       void onRemove() override;
   
       virtual Matrix& getViewMatrix();
       virtual Matrix& getProjectionMatrix();
       Vector3 getAbsolutePosition() const { return m_TransformComponent->getAbsoluteTransform().Translation(); }
       virtual const char* getName() const override { return "CameraComponent"; }
   
       PostProcessingDetails getPostProcessingDetails() const { return m_PostProcessingDetails; }
   
       ComponentID getComponentID() const { return s_ID; }
   
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
