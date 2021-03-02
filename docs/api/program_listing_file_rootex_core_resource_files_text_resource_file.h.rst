
.. _program_listing_file_rootex_core_resource_files_text_resource_file.h:

Program Listing for File text_resource_file.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_text_resource_file.h>` (``rootex\core\resource_files\text_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   
   class TextResourceFile : public ResourceFile
   {
   protected:
       String m_FileString;
   
       explicit TextResourceFile(const FilePath& path);
   
       friend class ResourceLoader;
   
   public:
       explicit TextResourceFile(TextResourceFile&) = delete;
       explicit TextResourceFile(TextResourceFile&&) = delete;
       virtual ~TextResourceFile() = default;
   
       void reimport() override;
       bool save() override;
   
       void putString(const String& newData);
       void popBack();
       void append(const String& add);
       String getString() const;
   };
