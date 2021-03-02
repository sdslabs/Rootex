
.. _program_listing_file_rootex_core_audio_static_audio_buffer.h:

Program Listing for File static_audio_buffer.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_audio_static_audio_buffer.h>` (``rootex\core\audio\static_audio_buffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "audio_buffer.h"
   
   class StaticAudioBuffer : public AudioBuffer
   {
       ALuint m_BufferID;
   
       void initializeBuffers() override;
       void destroyBuffers() override;
   
   public:
       StaticAudioBuffer(AudioResourceFile* audioFile);
       ~StaticAudioBuffer();
   
       ALuint& getBuffer();
   };
