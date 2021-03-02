
.. _program_listing_file_rootex_app_application_settings.h:

Program Listing for File application_settings.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_app_application_settings.h>` (``rootex\app\application_settings.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "core/resource_files/text_resource_file.h"
   
   class ApplicationSettings
   {
       static ApplicationSettings* s_Instance;
       TextResourceFile* m_TextSettingsFile;
       JSON::json m_Settings;
   
   public:
       static ApplicationSettings* GetSingleton() { return s_Instance; }
   
       ApplicationSettings(TextResourceFile* settingsFile);
       ApplicationSettings(ApplicationSettings&) = delete;
       ~ApplicationSettings();
   
       JSON::json::iterator find(const String& setting);
       JSON::json::iterator end();
   
       TextResourceFile* getTextFile() const { return m_TextSettingsFile; }
       JSON::json& getJSON() { return m_Settings; };
   };
