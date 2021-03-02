
.. _program_listing_file_rootex_core_language_locale.h:

Program Listing for File language_locale.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_language_locale.h>` (``rootex\core\language_locale.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   class Locale
   {
       HashMap<String, String> m_GameStrings;
       Locale() = default;
   
   public:
       static Locale* GetSingleton();
   
       String getString(const String key);
       void loadLanguage(const String location);
   };
