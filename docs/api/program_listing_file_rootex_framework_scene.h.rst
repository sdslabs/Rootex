
.. _program_listing_file_rootex_framework_scene.h:

Program Listing for File scene.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_scene.h>` (``rootex\framework\scene.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "entity.h"
   #include "core/input/input_manager.h"
   
   typedef unsigned int SceneID;
   
   #define ROOT_SCENE_ID 1
   
   struct SceneSettings
   {
       Vector<String> preloads = {};
       SceneID camera = ROOT_SCENE_ID;
       SceneID listener = ROOT_SCENE_ID;
       HashMap<String, InputScheme> inputSchemes;
       String startScheme = {};
   
       void drawSceneSelectables(Scene* scene, SceneID& toSet);
       void drawInputScheme(InputDescription& floatInput);
       void draw();
   };
   
   void to_json(JSON::json& j, const SceneSettings& s);
   void from_json(const JSON::json& j, SceneSettings& s);
   
   class Scene
   {
       static Vector<Scene*> s_Scenes;
   
       SceneID m_ID;
       String m_Name;
       String m_FullName;
       String m_SceneFile;
       Ptr<Entity> m_Entity;
       SceneSettings m_Settings;
   
       Scene* m_ParentScene = nullptr;
       Vector<Ptr<Scene>> m_ChildrenScenes;
   
       bool checkCycle(Scene* child);
   
   public:
       static void ResetNextID();
   
       static Ptr<Scene> Create(const JSON::json& sceneData);
       static Ptr<Scene> CreateFromFile(const String& sceneFile);
       static Ptr<Scene> CreateEmpty();
       static Ptr<Scene> CreateEmptyAtPath(const String& sceneFile);
       static Ptr<Scene> CreateEmptyWithEntity();
       static Ptr<Scene> CreateRootScene();
       static Vector<Scene*> FindScenesByName(const String& name);
   
       Scene(SceneID id, const String& name, const String& sceneFile, const SceneSettings& settings);
       ~Scene();
   
       Scene* findScene(SceneID scene);
       void reload();
   
       void onLoad();
       bool snatchChild(Scene* child);
       bool addChild(Ptr<Scene>& child);
       bool removeChild(Scene* toRemove);
   
       void setEntity(Ptr<Entity>& entity) { m_Entity = std::move(entity); }
       void setName(const String& name);
   
       JSON::json getJSON() const;
       Vector<Ptr<Scene>>& getChildren() { return m_ChildrenScenes; }
       SceneID getID() const { return m_ID; }
       Scene* getParent() const { return m_ParentScene; }
       Entity* getEntity() const { return m_Entity.get(); }
       const String& getSceneFilePath() const { return m_SceneFile; }
       const String& getName() const { return m_Name; }
       const String& getFullName() const { return m_FullName; }
       SceneSettings& getSettings() { return m_Settings; }
   };
