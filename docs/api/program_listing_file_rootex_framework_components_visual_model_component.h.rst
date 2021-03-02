
.. _program_listing_file_rootex_framework_components_visual_model_component.h:

Program Listing for File model_component.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_model_component.h>` (``rootex\framework\components\visual\model_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "components/transform_component.h"
   #include "core/resource_files/model_resource_file.h"
   #include "renderer/material.h"
   #include "renderer/mesh.h"
   #include "scene.h"
   #include "renderable_component.h"
   
   class ModelComponent : public RenderableComponent
   {
       static Component* Create(const JSON::json& componentData);
   
       friend class ECSFactory;
   
   protected:
       ModelResourceFile* m_ModelResourceFile;
   
       ModelComponent(unsigned int renderPass, ModelResourceFile* resFile, const HashMap<String, String>& materialOverrides, bool isVisible, const Vector<SceneID>& affectingStaticLightIDs);
       ModelComponent(ModelComponent&) = delete;
       virtual ~ModelComponent() = default;
   
       void assignBoundingBox();
       void assignOverrides(ModelResourceFile* newModel, const HashMap<String, String>& materialOverrides);
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::ModelComponent;
   
       static bool CompareMaterials(const Pair<Ref<Material>, Vector<Mesh>>& a, const Pair<Ref<Material>, Vector<Mesh>>& b);
       virtual bool setupData() override;
   
       virtual bool preRender(float deltaMilliseconds) override;
       virtual void render() override;
   
       void setModelResourceFile(ModelResourceFile* newModel, const HashMap<String, String>& materialOverrides);
   
       const Vector<Pair<Ref<Material>, Vector<Mesh>>>& getMeshes() const { return m_ModelResourceFile->getMeshes(); }
       ModelResourceFile* getModelResourceFile() const { return m_ModelResourceFile; }
   
       virtual const char* getName() const override { return "ModelComponent"; }
       ComponentID getComponentID() const override { return s_ID; }
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
