
.. _program_listing_file_rootex_framework_systems_post_process_system.h:

Program Listing for File post_process_system.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_post_process_system.h>` (``rootex\framework\systems\post_process_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   #include "system.h"
   
   #include "core/renderer/post_processor.h"
   
   class PostProcessSystem : public System
   {
       PostProcessor m_Processor;
       PostProcessingDetails m_PostProcessingDetails;
   
       PostProcessSystem();
       PostProcessSystem(const PostProcessSystem&) = delete;
       ~PostProcessSystem() = default;
   
   public:
       static PostProcessSystem* GetSingleton();
   
       void update(float deltaMilliseconds) override;
   
       void addCustomPostProcessing(const String& path);
   };
