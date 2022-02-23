
.. _program_listing_file_rootex_core_resource_file.h:

Program Listing for File resource_file.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_file.h>` (``rootex\core\resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   #include "os/os.h"
   
   class ResourceFile
   {
   public:
       enum class Type : int
       {
           None = 0,
           Lua = 1,
           Audio = 2,
           Text = 3,
           Model = 4,
           AnimatedModel = 5,
           CollisionModel = 6,
           Image = 7,
           ImageCube = 8,
           Font = 9,
           ParticleEffect = 10,
           BasicMaterial = 11,
           InstancingBasicMaterial = 12,
           AnimatedBasicMaterial = 13,
           SkyMaterial = 14,
           CustomMaterial = 15,
           DecalMaterial = 16
       };
   
       static const inline Map<Type, const String> s_TypeNames = {
           { Type::None, "None" },
           { Type::Lua, "Lua" },
           { Type::Audio, "Audio" },
           { Type::Text, "Text" },
           { Type::Model, "Model" },
           { Type::AnimatedModel, "AnimatedModel" },
           { Type::CollisionModel, "CollisionModel" },
           { Type::Image, "Image" },
           { Type::ImageCube, "ImageCube" },
           { Type::Font, "Font" },
           { Type::ParticleEffect, "ParticleEffect" },
           { Type::BasicMaterial, "BasicMaterial" },
           { Type::InstancingBasicMaterial, "InstancingBasicMaterial" },
           { Type::AnimatedBasicMaterial, "AnimatedBasicMaterial" },
           { Type::SkyMaterial, "SkyMaterial" },
           { Type::CustomMaterial, "CustomMaterial" },
           { Type::DecalMaterial, "DecalMaterial" }
       };
   
       Type m_Type;
       FilePath m_Path;
       FileTimePoint m_LastReadTime;
       FileTimePoint m_LastChangedTime;
   
   protected:
       explicit ResourceFile(const Type& type, const FilePath& path);
   
       friend class ResourceLoader;
   
   public:
       explicit ResourceFile(const ResourceFile&) = delete;
       explicit ResourceFile(const ResourceFile&&) = delete;
       virtual ~ResourceFile() = default;
   
       virtual void reimport();
       virtual bool save();
       virtual void draw();
   
       bool isDirty();
   
       FilePath getPath() const;
       Type getType() const;
       const FileTimePoint& getLastReadTime() const { return m_LastReadTime; }
       const FileTimePoint& getLastChangedTime();
   };
   
   typedef Vector<Pair<ResourceFile::Type, String>> ResourceCollection;
   
   void to_json(JSON::json& j, const ResourceFile::Type& t);
   void from_json(const JSON::json& j, ResourceFile::Type& s);
