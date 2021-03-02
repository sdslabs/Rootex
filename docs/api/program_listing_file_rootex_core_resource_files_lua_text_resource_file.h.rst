
.. _program_listing_file_rootex_core_resource_files_lua_text_resource_file.h:

Program Listing for File lua_text_resource_file.h
=================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_lua_text_resource_file.h>` (``rootex\core\resource_files\lua_text_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "text_resource_file.h"
   
   class LuaTextResourceFile : public TextResourceFile
   {
       explicit LuaTextResourceFile(const FilePath& path);
   
       friend class ResourceLoader;
   
   public:
       explicit LuaTextResourceFile(TextResourceFile&) = delete;
       explicit LuaTextResourceFile(TextResourceFile&&) = delete;
       ~LuaTextResourceFile() = default;
   };
