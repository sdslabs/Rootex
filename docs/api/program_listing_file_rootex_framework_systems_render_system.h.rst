
.. _program_listing_file_rootex_framework_systems_render_system.h:

Program Listing for File render_system.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_render_system.h>` (``rootex\framework\systems\render_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "core/renderer/renderer.h"
   #include "core/renderer/render_pass.h"
   #include "main/window.h"
   #include "framework/ecs_factory.h"
   #include "framework/scene.h"
   #include "framework/system.h"
   #include "framework/components/visual/camera_component.h"
   #include "components/visual/model_component.h"
   #include "components/visual/animated_model_component.h"
   
   #include "ASSAO/ASSAO.h"
   
   #define LINE_INITIAL_RENDER_CACHE 1000
   
   class RenderSystem : public System
   {
       struct LineRequests
       {
           Vector<float> m_Endpoints;
           Vector<unsigned short> m_Indices;
       };
   
       CameraComponent* m_Camera;
   
       Ptr<Renderer> m_Renderer;
       Vector<Matrix> m_TransformationStack;
   
       Ref<BasicMaterial> m_LineMaterial;
       LineRequests m_CurrentFrameLines;
   
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSPerFrameConstantBuffer;
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSProjectionConstantBuffer;
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSPerFrameConstantBuffer;
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSPerLevelConstantBuffer;
   
       bool m_IsEditorRenderPassEnabled;
   
       RenderSystem();
       RenderSystem(RenderSystem&) = delete;
   
       void renderPassRender(float deltaMilliseconds, RenderPass renderPass);
   
       template <class T>
       void renderComponents(float deltaMilliseconds, RenderPass renderPass);
   
       Variant onOpenedScene(const Event* event);
   
   public:
       static RenderSystem* GetSingleton();
   
       void setConfig(const SceneSettings& sceneSettings) override;
       void update(float deltaMilliseconds) override;
       void renderLines();
   
       void submitLine(const Vector3& from, const Vector3& to);
       void submitBox(const Vector3& min, const Vector3& max);
       void submitSphere(const Vector3& center, const float& radius);
       void submitCone(const Matrix& transform, const float& height, const float& radius);
   
       void recoverLostDevice();
   
       void setCamera(CameraComponent* camera);
       void restoreCamera();
   
       void calculateTransforms(Scene* scene);
       void pushMatrix(const Matrix& transform);
       void pushMatrixOverride(const Matrix& transform);
       void popMatrix();
   
       void enableWireframeRasterizer();
       void resetDefaultRasterizer();
   
       void setProjectionConstantBuffers();
       void perFrameVSCBBinds(float fogStart, float fogEnd);
       void perFramePSCBBinds(const Color& fogColor);
       void perScenePSCBBinds();
       void updatePerSceneBinds();
   
       void setIsEditorRenderPass(bool enabled) { m_IsEditorRenderPassEnabled = enabled; }
   
       void enableLineRenderMode();
       void resetRenderMode();
   
       CameraComponent* getCamera() const { return m_Camera; }
       const Matrix& getCurrentMatrix() const;
       Renderer* getRenderer() const { return m_Renderer.get(); }
   
       void draw() override;
   };
   
   template <class T>
   inline void RenderSystem::renderComponents(float deltaMilliseconds, RenderPass renderPass)
   {
       for (auto& c : ECSFactory::GetComponents<T>())
       {
           T* tc = (T*)c;
           if (tc->getRenderPass() & (unsigned int)renderPass)
           {
               tc->preRender(deltaMilliseconds);
               if (tc->isVisible())
               {
                   tc->render();
               }
               tc->postRender();
           }
       }
   }
