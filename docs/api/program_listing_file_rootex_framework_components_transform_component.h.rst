
.. _program_listing_file_rootex_framework_components_transform_component.h:

Program Listing for File transform_component.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_transform_component.h>` (``rootex\framework\components\transform_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "component.h"
   
   class TransformComponent : public Component
   {
   private:
       static Component* Create(const JSON::json& componentData);
   
       struct TransformBuffer
       {
           Vector3 m_Position;
           Quaternion m_Rotation;
           Vector3 m_Scale;
           BoundingBox m_BoundingBox;
   
           Matrix m_Transform;
       };
       TransformBuffer m_TransformBuffer;
   
       Matrix m_ParentAbsoluteTransform;
       bool m_LockScale = false;
   
       const TransformBuffer* getTransformBuffer() const { return &m_TransformBuffer; };
   
       void updateTransformFromPositionRotationScale();
       void updatePositionRotationScaleFromTransform(Matrix& transform);
   
       TransformComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const BoundingBox& bounds);
       TransformComponent(TransformComponent&) = delete;
   
       friend class ModelComponent;
       friend class RenderSystem;
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformComponent;
   
       virtual ~TransformComponent() = default;
   
       void setPosition(const Vector3& position);
       void setRotation(const float& yaw, const float& pitch, const float& roll);
       void setRotationQuaternion(const Quaternion& rotation);
       void setScale(const Vector3& scale);
       void setTransform(const Matrix& transform);
       void setAbsoluteTransform(const Matrix& transform);
       void setBounds(const BoundingBox& bounds);
       void setRotationPosition(const Matrix& transform);
       void setAbsoluteRotationPosition(const Matrix& transform);
       void setParentAbsoluteTransform(const Matrix& parentTransform);
   
       void addTransform(const Matrix& applyTransform);
       void addRotation(const Quaternion& applyTransform);
   
       Vector3 getPosition() const { return m_TransformBuffer.m_Position; }
       BoundingBox getWorldSpaceBounds() const;
       Quaternion getRotation() const { return m_TransformBuffer.m_Rotation; };
       const Vector3& getScale() const { return m_TransformBuffer.m_Scale; }
       const Matrix& getLocalTransform() const { return m_TransformBuffer.m_Transform; }
       Matrix getRotationPosition() const { return Matrix::CreateFromQuaternion(m_TransformBuffer.m_Rotation) * Matrix::CreateTranslation(m_TransformBuffer.m_Position) * m_ParentAbsoluteTransform; }
       Matrix getAbsoluteTransform() const { return m_TransformBuffer.m_Transform * m_ParentAbsoluteTransform; }
       Matrix getParentAbsoluteTransform() const { return m_ParentAbsoluteTransform; }
       ComponentID getComponentID() const override { return s_ID; }
       virtual const char* getName() const override { return "TransformComponent"; }
       virtual JSON::json getJSON() const override;
   
       void draw() override;
       void highlight();
   };
