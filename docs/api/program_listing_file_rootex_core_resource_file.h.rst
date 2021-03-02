
.. _program_listing_file_rootex_core_resource_file.h:

Program Listing for File resource_file.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_file.h>` (``rootex\core\resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   class ResourceFile
   {
   public:
       enum class Type : int
       {
           None = 0,
           Lua,
           Audio,
           Text,
           Model,
           AnimatedModel,
           CollisionModel,
           Image,
           ImageCube,
           Font
       };
   
   protected:
       Type m_Type;
       FilePath m_Path;
       FileTimePoint m_LastReadTime;
       FileTimePoint m_LastChangedTime;
   
       explicit ResourceFile(const Type& type, const FilePath& path);
   
       friend class ResourceLoader;
   
   public:
       explicit ResourceFile(const ResourceFile&) = delete;
       explicit ResourceFile(const ResourceFile&&) = delete;
       virtual ~ResourceFile() = default;
   
       virtual void reimport();
       virtual bool save();
   
       bool isDirty();
   
       FilePath getPath() const;
       Type getType() const;
       const FileTimePoint& getLastReadTime() const { return m_LastReadTime; }
       const FileTimePoint& getLastChangedTime();
   };
