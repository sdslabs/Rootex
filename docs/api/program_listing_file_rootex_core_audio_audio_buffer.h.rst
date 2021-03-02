
.. _program_listing_file_rootex_core_audio_audio_buffer.h:

Program Listing for File audio_buffer.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_audio_audio_buffer.h>` (``rootex\core\audio\audio_buffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "core/resource_files/audio_resource_file.h"
   
   class AudioBuffer
   {
   protected:
       AudioResourceFile* m_AudioFile;
   
       AudioBuffer(AudioResourceFile* audioFile);
   
       virtual void initializeBuffers() = 0;
       virtual void destroyBuffers() = 0;
   
   public:
       AudioBuffer(AudioBuffer&) = default;
       virtual ~AudioBuffer();
   
       AudioResourceFile* getAudioFile();
   };
